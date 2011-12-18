/*
 * chess.h
 *
 *  Created on: 10.7.2011
 *      Author: martin
 */

#ifndef CHESS4UZEBOX_H_
#define CHESS4UZEBOX_H_

// piece names
#define PIECE_TYPE_NONE (0)
#define PIECE_TYPE_PAWN (1)
#define PIECE_TYPE_ROOK (2)
#define PIECE_TYPE_KNIGHT (3)
#define PIECE_TYPE_BISHOP (4)
#define PIECE_TYPE_QUEEN (5)
#define PIECE_TYPE_KING (6)

#define PIECE_COLOR_WHITE (0)
#define PIECE_COLOR_BLACK (1)
#define SQUARE_COLOR_WHITE (0)
#define SQUARE_COLOR_BLACK (1)

// mapping pieces to tiles [piece color][tile color][piece type]=>[map]
const char pieces_map[2][2][7][11] PROGMEM = {
	{
		{
			{3, 3, 171, 171, 171, 171, 171, 171, 171, 171, 171},
			{3, 3, 1, 2, 3, 16, 17, 18, 31, 32, 33},
			{3, 3, 4, 5, 6, 19, 20, 21, 34, 35, 36},
			{3, 3, 7, 8, 9, 22, 23, 24, 34, 35, 36},
			{3, 3, 10, 11, 12, 25, 26, 27, 34, 35, 36},
			{3, 3, 13, 14, 15, 28, 29, 30, 34, 35, 36},
			{3, 3, 37, 38, 39, 28, 29, 30, 34, 35, 36}
		},
		{
			{3, 3, 172, 172, 172, 172, 172, 172, 172, 172, 172},
			{3, 3, 46, 47, 48, 61, 17, 62, 152, 153, 154},
			{3, 3, 49, 50, 51, 63, 20, 64, 43, 44, 45},
			{3, 3, 52, 53, 54, 65, 66, 67, 43, 44, 45},
			{3, 3, 55, 56, 57, 68, 26, 69, 43, 44, 45},
			{3, 3, 58, 59, 60, 70, 29, 71, 43, 44, 45},
			{3, 3, 40, 41, 42, 70, 29, 71, 43, 44, 45}
		}
	},
	{
		{
			{3, 3, 171, 171, 171, 171, 171, 171, 171, 171, 171},
			{3, 3, 72, 73, 74, 87, 88, 89, 102, 103, 104},
			{3, 3, 75, 76, 77, 90, 91, 92, 105, 106, 107},
			{3, 3, 78, 79, 80, 93, 94, 95, 105, 106, 107},
			{3, 3, 81, 82, 83, 96, 97, 98, 105, 106, 107},
			{3, 3, 84, 85, 86, 99, 100, 101, 105, 106, 107},
			{3, 3, 108, 109, 110, 99, 100, 101, 105, 106, 107}
		},
		{
			{3, 3, 172, 172, 172, 172, 172, 172, 172, 172, 172},
			{3, 3, 117, 118, 119, 132, 88, 133, 155, 156, 157},
			{3, 3, 120, 121, 122, 134, 91, 135, 155, 156, 157},
			{3, 3, 123, 124, 125, 136, 137, 138, 155, 156, 157},
			{3, 3, 126, 127, 128, 139, 97, 140, 155, 156, 157},
			{3, 3, 129, 130, 131, 141, 100, 142, 155, 156, 157},
			{3, 3, 111, 112, 113, 141, 100, 142, 155, 156, 157}
		}
	}
};

// blinking piece map
const char blink_piece_map[11] PROGMEM = {3, 3, 173, 173, 173, 173, 173, 173, 173, 173, 173};

// screen layout
#define BOARD_OFFSET_X (3)
#define BOARD_OFFSET_Y (2)
#define BOARD_SQUARE_SIZE_X (3)
#define BOARD_SQUARE_SIZE_Y (3)
#define BOARD_PROMOTION_X (8)
#define BOARD_PROMOTION_Y (12)

// sounds (sound events)
#define SOUND_FX_OFFSET (31)
#define SOUND_MOVE_CURSOR (0 + SOUND_FX_OFFSET)
#define SOUND_CANT_MOVE_CURSOR (1 + SOUND_FX_OFFSET)
#define SOUND_SELECT (2 + SOUND_FX_OFFSET)
#define SOUND_MOVE_ERROR (3 + SOUND_FX_OFFSET)
#define SOUND_CAPTURE (5 + SOUND_FX_OFFSET)
#define SOUND_PROMOTION (6 + SOUND_FX_OFFSET)
#define SOUND_CHECK (7 + SOUND_FX_OFFSET)
#define SOUND_MATE (8 + SOUND_FX_OFFSET)
#define SOUND_DRAW (9 + SOUND_FX_OFFSET)
#define SOUND_COMPUTER_MOVE (4 + SOUND_FX_OFFSET)

// settings
#define SETTINGS_SOUND (1<<0)
#define SETTINGS_MUSIC (1<<1)
#define SETTINGS_TURBO (1<<2)

// fonts
#define MENU_FONT_START (108)
#define GAME_FONT_START (32)
#define GAME_FONT_0 (174)
#define GAME_FONT_A (GAME_FONT_0 + 10)
#define GAME_FONT_QUEEN (GAME_FONT_A + 8)
#define GAME_FONT_KNIGHT (GAME_FONT_QUEEN + 1)
#define GAME_FONT_ROOK (GAME_FONT_QUEEN + 2)
#define GAME_FONT_BISHOP (GAME_FONT_QUEEN + 3)
#define GAME_FONT_CHECK (GAME_FONT_QUEEN + 4)
#define GAME_FONT_HALF (GAME_FONT_QUEEN + 5)
#define GAME_FONT_D (GAME_FONT_A + 3)
#define GAME_FONT_E (GAME_FONT_A + 4)
#define GAME_FONT_M (198)
#define GAME_FONT_O (GAME_FONT_M + 1)


// eeprom constants
#define EEPR_BLOCK_ID (128)
#define EEPR_PMODE_POS (0)
#define EEPR_SETTINGS_POS (1)
#define EEPR_RANDOM1_POS (2)
#define EEPR_RANDOM2_POS (3)

// time, before running demo from menu
#define DEMO_COUNTDOWN_TOP (600)

// names for player variants
const char players[6][17] PROGMEM = {
	"Human (joypad 1)",
	"Human (joypad 2)",
	"CPU (easy)",
	"CPU (medium)",
	"CPU (hard)",
	"CPU (extreme)"
};

// names for sound/music/turbo variants
const char yesno[2][4] PROGMEM = {
	"No",
	"Yes"
};


// function headers
void play_sound(uint8_t sound_event);
void show_menu();
void init_game();
uint8_t select_square(uint8_t *cursor, unsigned char joypadNo);
void read_move();
void draw_piece(uint8_t x, uint8_t y, uint8_t piece_type, uint8_t piece_color);
void draw_board();
uint8_t print_log();
void start_thinking();
void stop_thinking();
int main();

#endif /* CHESS4UZEBOX_H_ */
