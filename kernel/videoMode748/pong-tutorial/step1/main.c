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

#define SPI_VRAM     0x0000U          /* background tile map: 24 rows x 25 bytes */

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

	SetRenderingParameters(32, ROWS * 8);
	m74_rows   = (u16)rowsel;
	m74_vaddr  = (u16)row_table;
	m74_rtbase = RT_BASE;
	m74_rtmax  = RT_MAX;
	M74_VramRestore();
	m74_config |= M74_CFG_ENABLE;                    /* switch the display on */
}

int main(void)
{
	if (SpiRamInit() == 0) { while (1) { } }     /* no SPI RAM: stop here */
	setup_video();

	while (1) {
		WaitVsync(1);                            /* wait for the next frame */
	}
}
