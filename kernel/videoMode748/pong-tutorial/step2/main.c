/* Pong for the Uzebox, video mode 748 - a beginner's tutorial game */
#include <stdbool.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <uzebox.h>
#include <spiram.h>

/* ---------------------------------------------------------------- graphics */

/* Two 4-bit pixels packed into one byte: the left pixel in the high half */
#define PX(a, b)  (((a) << 4) | (b))

/* Background tiles (8 x 8 pixels, 4 bytes per row, 32 bytes per tile).
** They stay in flash; Mode 748 needs them to start on a 256-byte boundary. */
static const u8 bg_tiles[3 * 32] __attribute__((aligned(256))) PROGMEM = {
	/* tile 0: empty */
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* tile 1: a dash of the net, in colour 2 */
	PX(0,0),PX(0,2),PX(2,0),PX(0,0),  PX(0,0),PX(0,2),PX(2,0),PX(0,0),
	PX(0,0),PX(0,2),PX(2,0),PX(0,0),  PX(0,0),PX(0,2),PX(2,0),PX(0,0),
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	/* tile 2: wall, in colour 1 */
	0,0,0,0, 0,0,0,0, 0x11,0x11,0x11,0x11, 0x11,0x11,0x11,0x11,
	0x11,0x11,0x11,0x11, 0x11,0x11,0x11,0x11, 0,0,0,0, 0,0,0,0,
};
#define T_EMPTY 0
#define T_NET   1
#define T_WALL  2

/* The sprite system recolours every pixel through this table; an identity
** table means "draw the colours as they are". The Makefile places it at
** M74_RECTB_OFF (0xE000). */
const u8 recolor[16] __attribute__((section(".rectb"))) =
	{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

/* Sprite tiles are made at start-up and copied into SPI RAM */
#define SPI_VRAM     0x0000U          /* background tile map: 24 rows x 25 bytes */
#define SPI_SPRITES  0x1000U          /* sprite tiles, 32 bytes each */
#define SPR_PADDLE   0
#define SPR_BALL     1

/* ------------------------------------------------------------------- video */

#define ROWS 24                       /* 24 tile rows of 8 lines = 192 lines */

static m74_mode0_vram_t rows[ROWS];   /* one control block per tile row */
static u8 rowsel[] = { 0, 255 };      /* start the display at logical line 0 */

#define R(n) ((u16)(&rows[(n) % ROWS]))
static const u16 row_table[32] PROGMEM = {
	R(0), R(1), R(2), R(3), R(4), R(5), R(6), R(7), R(8), R(9), R(10), R(11),
	R(12), R(13), R(14), R(15), R(16), R(17), R(18), R(19), R(20), R(21), R(22), R(23),
	R(24), R(25), R(26), R(27), R(28), R(29), R(30), R(31)
};

/* Make one 8 x 8 sprite tile from a function telling the colour of each pixel */
static void make_tile(u8 number, u8 (*pixel)(u8 x, u8 y, u8 arg), u8 arg)
{
	u8 tile[32];
	u8 x, y;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x += 2) {
			tile[y * 4 + x / 2] = PX(pixel(x, y, arg), pixel(x + 1, y, arg));
		}
	}
	SpiRamWriteFrom(0, SPI_SPRITES + number * 32U, tile, 32);
}

static u8 paddle_px(u8 x, u8 y, u8 arg) { return (x >= 2 && x <= 5) ? 1 : 0; }
static u8 ball_px(u8 x, u8 y, u8 arg)
{
	bool corner = (x == 1 || x == 6) && (y == 1 || y == 6);
	return (x >= 1 && x <= 6 && y >= 1 && y <= 6 && !corner) ? 3 : 0;
}
static void setup_video(void)
{
	u8 r, c, line[25];
	u8* palette = (u8*)m74_paddr;

	/* colours: 0 black, 1 white, 2 grey, 3 yellow (Uzebox colours are BBGGGRRR) */
	palette[0] = 0x00; palette[1] = 0xFF; palette[2] = 0x52; palette[3] = 0x3F;

	for (r = 0; r < ROWS; r++) {
		for (c = 0; c < 25; c++) {
			if (r == 0 || r == ROWS - 1) { line[c] = T_WALL; }
			else if (c == 12)            { line[c] = T_NET; }
			else                         { line[c] = T_EMPTY; }
		}
		SpiRamWriteFrom(0, SPI_VRAM + r * 25U, line, 25);
		rows[r].config    = 0;                       /* row mode 0: tiles */
		rows[r].bg_addr   = SPI_VRAM + r * 25U;      /* its tile map in SPI RAM */
		rows[r].t0_addr_h = (u16)bg_tiles >> 8;      /* where its tiles are */
		rows[r].t1_addr_h = (u16)bg_tiles >> 8;
	}

	make_tile(SPR_PADDLE, paddle_px, 0);
	make_tile(SPR_BALL, ball_px, 0);

	SetRenderingParameters(32, ROWS * 8);
	m74_rows   = (u16)rowsel;
	m74_vaddr  = (u16)row_table;
	m74_rtbase = RT_BASE;
	m74_rtmax  = RT_MAX;
	M74_VramRestore();
	m74_config |= M74_CFG_ENABLE;                    /* switch the display on */
}

/* Draw a sprite tile with its top left corner at (x, y) */
static void sprite(u8 number, u8 x, u8 y)
{
	M74_BlitSprite(SPI_SPRITES + number * 32U, x + 8, y + 8, 0);
}

/* -------------------------------------------------------------------- game */

#define TOP      8                    /* the court, inside the walls */
#define BOTTOM   184
#define PADDLE_H 32                   /* 4 sprite tiles tall */
#define LEFT_X   8                    /* sprite x of each paddle */
#define RIGHT_X  176

u8 left_y = 80, right_y = 80;         /* paddle tops */
int ball_x = 92, ball_y = 92;         /* ball position */

static void draw_paddle(u8 x, u8 y)
{
	u8 i;
	for (i = 0; i < 4; i++) { sprite(SPR_PADDLE, x, y + i * 8); }
}

static void draw(void)
{
	M74_VramRestore();                           /* remove last frame's sprites */
	draw_paddle(LEFT_X, left_y);
	draw_paddle(RIGHT_X, right_y);
	sprite(SPR_BALL, ball_x, ball_y);
}

int main(void)
{
	if (SpiRamInit() == 0) { while (1) { } }     /* no SPI RAM: stop here */
	setup_video();

	while (1) {
		WaitVsync(1);                            /* wait for the next frame */
		draw();
	}
}
