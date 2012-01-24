#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <uzebox.h>

#include "data/sokoban_tiles.c"
#include "data/sokoban_tile_names.h"
#include "data/sokoban_levels.h"
#include "data/sokoban_sound.h"

// index of current level
uint16_t sokoban_level;
// info, if current level have been done
uint8_t sokoban_level_done;

// pointer to start of this level in program memory
char* sokoban_level_data_pointer;
// pointer to start of next level in program memory
char* sokoban_level_data_pointer_next;

// number of moves that sokoban did (not displayed)
uint16_t sokoban_moves;
// number of pushes that sokoban did (not displayed)
uint16_t sokoban_pushes;
// number of moves that sokoban did since start of level
uint16_t sokoban_moves_level;
// number of pushes that sokoban did since start of level
uint16_t sokoban_pushes_level;


// undo push ringbuffer
// structure describing one step back in push history
typedef struct {
	uint8_t pos; // position where player was before push
	int8_t offset; // offset in vram (level data) where were pushed
} sokoban_undo_ringbuffer_type;
// array of structures (ringbuffer)
sokoban_undo_ringbuffer_type sokoban_undo_ringbuffer[SOKOBAN_UNDO_RINGBUFFER_SIZE];
// current position (index) in ringbuffer
uint16_t sokoban_undo_ringbuffer_pos;
// count of entries in ringbuffer
uint16_t sokoban_undo_ringbuffer_count;

// emulating TVText functions
// text positioning (window parameters and cursor position)
uint8_t text_pos_x1, text_pos_y1, text_pos_x2, text_pos_y2, text_pos_x, text_pos_y;

/**
 * Sets window position
 * @param uint8_t x1 left margin of window
 * @param uint8_t y1 top margin of window
 * @param uint8_t x2 right margin of window
 * @param uint8_t y2 bottom margin of window
 */
void text_set_window(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	text_pos_x1 = x1;
	text_pos_y1 = y1;
	text_pos_x2 = x2;
	text_pos_y2 = y2;
	text_pos_x = x1;
	text_pos_y = y1;
}

/**
 * Puts character into vram (screen)
 * @param uint8_t c character to print
 */
void text_putc(uint8_t c) {
	vram[text_pos_x + text_pos_y * SCREEN_TILES_H] = c;
	text_pos_x++;
	if (text_pos_x > text_pos_x2) {
		text_pos_x = text_pos_x1;
		text_pos_y++;
		if (text_pos_y > text_pos_y2) {
			text_pos_y = text_pos_y1;
		}
	}
}

/**
 * Print number formatted in base of 10 (with leading zeroes - fixed length)
 * @param uint16_t number number to print
 * @param uint8_t digits number of digits to print
 */
void text_print_number(uint16_t number, uint8_t digits) {
	for (uint8_t i = digits; i > 0; i--) {
		uint16_t mul = 1;
		for (uint8_t j = 1; j < i; j++) {
			mul *= 10;
		}
		text_putc(((number / mul) % 10) + FONT_SOKOBAN_0);
	}
}

/**
 * Advances text cursor to beginning of next line. At the bottom wraps to the top.
 */
void text_next_line() {
	text_pos_x = text_pos_x1;
	text_pos_y++;
	if (text_pos_y > text_pos_y2) {
		text_pos_y = text_pos_y1;
	}
}

/**
 * Writes information into EEPROM, that given level has been done.
 * @param uint16_t level index of level to mark as done
 */
void eeprom_set_level_done(uint16_t level) {
	struct EepromBlockStruct eeprom_data;

	uint16_t block_id = level / 30 / 8;
	uint8_t byte_id = (level / 8) % 30;
	uint8_t bit_id = level % 8;

	if (!EepromReadBlock(130 + block_id, &eeprom_data) == 0) {
		eeprom_data.id = 130 + block_id;
		for (uint8_t i = 0; i < 30; i++) {
			eeprom_data.data[i] = 0;
		}
	}
	eeprom_data.data[byte_id] |= (1 << bit_id);

	EepromWriteBlock(&eeprom_data);
}

/**
 * Reads from EEPROM if given level has been done.
 * @param uint16_t level index of level, which we want to know, if has been done
 * @return int8_t true if level has been done, otherwise false
 */
uint8_t eeprom_is_level_done(uint16_t level) {
	struct EepromBlockStruct eeprom_data;

	uint16_t block_id = level / 30 / 8;
	uint8_t byte_id = (level / 8) % 30;
	uint8_t bit_id = level % 8;

	if (EepromReadBlock(130 + block_id, &eeprom_data) == 0) {
		if (eeprom_data.data[byte_id] & (1 << bit_id)) {
			return 1;
		}
	}

	return 0;
}

/**
 * Saves into EEPROM last played level.
 * @param uint16_t level index of level to save
 */
void eeprom_set_last_level(uint16_t level) {
	struct EepromBlockStruct eeprom_data;
	if (!EepromReadBlock(134, &eeprom_data) == 0) {
		eeprom_data.id = 134;
		for (uint8_t i = 0; i < 30; i++) {
			eeprom_data.data[i] = 0;
		}
	}
	eeprom_data.data[29] = level >> 8;
	eeprom_data.data[28] = level & 0xff;

	EepromWriteBlock(&eeprom_data);
}

/**
 * Reads from EEPROM last played level
 * @return uint16_t index of level last played
 */
uint16_t eeprom_get_last_level() {
	struct EepromBlockStruct eeprom_data;

	if (EepromReadBlock(134, &eeprom_data) == 0) {
		uint16_t last_level = eeprom_data.data[29] << 8 | eeprom_data.data[28];
		if (last_level < SOKOBAN_LEVELS_COUNT) {
			return last_level;
		}
	}

	return 0;
}

/**
 * Draws info about level (level status, number and statistics)
 */
void sokoban_draw_info() {
	text_set_window(0, SCREEN_TILES_V-1, SCREEN_TILES_H-1, SCREEN_TILES_V-1);
	text_putc(sokoban_level_done ? FONT_SOKOBAN_TICK_YES : FONT_SOKOBAN_TICK_NO);
	text_putc(FONT_SOKOBAN_LVL);
	text_print_number(sokoban_level + 1, 3);
	text_putc(FONT_SOKOBAN_PUSH);
	text_print_number(sokoban_pushes_level, 4);
	text_putc(FONT_SOKOBAN_MOVE);
	text_print_number(sokoban_moves_level, 5);
}

/**
 * Loads and draws level data (field and info) on screen
 */
void sokoban_load_level() {
	// position of "reading head" in level data (program memory)
	char* reading_pos;
	// read token, and his first and second nibble
	char token, token1, token2;
	// dimensions of level
	uint8_t level_width, level_height;

	// clear level statistics
	sokoban_moves_level = 0;
	sokoban_pushes_level = 0;

	// clear undo push
	sokoban_undo_ringbuffer_pos = 0;
	sokoban_undo_ringbuffer_count = 0;

	// set position of head to start of level data
	reading_pos = sokoban_level_data_pointer;

	// clear display
	for (uint8_t i = 0; i < 16*12; i++) {
		vram[i] = FONT_SOKOBAN_SKY;
	}

	// read dimensions and set viewport
	token = pgm_read_byte(reading_pos);
	reading_pos++;
	level_width = ((token & 0b11110000) >> 4) + 1;
	level_height = (token & 0b00001111) + 1;

	// centering
	text_set_window(
		(SOKOBAN_FIELD_LEFT + SOKOBAN_FIELD_RIGHT - level_width) / 2 ,
		(SOKOBAN_FIELD_TOP + SOKOBAN_FIELD_BOTTOM - level_height) / 2,
		(SOKOBAN_FIELD_LEFT + SOKOBAN_FIELD_RIGHT - level_width) / 2 + level_width - 1,
		(SOKOBAN_FIELD_TOP + SOKOBAN_FIELD_BOTTOM - level_height) / 2 + level_height - 1
	);

	// read level data and write to screen (compression algorithm is described in sokoban_levels.h)
	do {
		token = pgm_read_byte(reading_pos);
		token1 = ((token & 0b11110000) >> 4);
		token2 = (token & 0b00001111);

		if (token1 < 0xe) {
			text_putc(FONT_SOKOBAN_FLOOR + (token1 & 0b00000111));
			if (token1 > 0x7) {
				text_putc(FONT_SOKOBAN_FLOOR + (token1 & 0b00000111));
			}
		} else {
			text_next_line();
		}
		if (token2 < 0xe) {
			text_putc(FONT_SOKOBAN_FLOOR + (token2 & 0b00000111));
			if (token2 > 0x7) {
				text_putc(FONT_SOKOBAN_FLOOR + (token2 & 0b00000111));
			}
		} else {
			text_next_line();
		}

		reading_pos++;
	} while ((token1 != 0b00001111) && (token2 != 0b00001111));

	// change skies to floors in game area
	for (uint8_t i = text_pos_x1; i <= text_pos_x2; i++) {
		for (uint8_t j = text_pos_y1; j <= text_pos_y2; j++) {
			if (vram[i + j * SCREEN_TILES_H] == FONT_SOKOBAN_SKY) {
				vram[i + j * SCREEN_TILES_H] = FONT_SOKOBAN_FLOOR;
			}
		}
	}

	// get level completion status
	sokoban_level_done = eeprom_is_level_done(sokoban_level);

	// set pointer to next level
	sokoban_level_data_pointer_next = reading_pos;

	// show info
	sokoban_draw_info();
}

/**
 * Restarts whole game (clears counters and starts at first level)
 */
void sokoban_restart_game() {
	sokoban_level = 0;
	sokoban_level_data_pointer = (char*)sokoban_levels;
	sokoban_moves = 0;
	sokoban_pushes = 0;
}

/**
 * Advances to the next level
 */
void sokoban_advance_level() {
	sokoban_level++;
	sokoban_level_data_pointer = sokoban_level_data_pointer_next;

	if (sokoban_level >= SOKOBAN_LEVELS_COUNT) {
		sokoban_restart_game();
	}
}

/**
 * Skips all levels from the beginning to level before current
 */
void sokoban_skip_to_previous_level() {
	SetRenderingParameters(1, 1);

	uint16_t to_level;
	if (sokoban_level > 0) {
		to_level = (sokoban_level - 1);
	} else {
		to_level = SOKOBAN_LEVELS_COUNT - 1;
	}
	sokoban_restart_game();
	while (sokoban_level < to_level) {
		sokoban_load_level();
		sokoban_advance_level();
	}
	sokoban_load_level();

	SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);
}


/**
 * Returns pointer to char with player
 * @return char* pointer on tile in tvtext_buffer, where sokoban is
 */
inline unsigned char* sokoban_find_player() {
	unsigned char* a;

	// iterate through tvtext_buffer and search for sokoban or sokoban on target
	for (a = vram; a < vram + SCREEN_TILES_H * SCREEN_TILES_V; a++) {
		if ((*a == FONT_SOKOBAN_PLAYER) || (*a == FONT_SOKOBAN_PLAYER_TARGET)) {
			return a;
		}
	}

	return NULL;
}

/**
 * Returns if level is cleared
 * @return uint8_t 1 if level is cleared (done), 0 otherwise
 */
inline uint8_t sokoban_is_level_cleared() {
	unsigned char* a;

	// iterate through tvtext buffer and return 0 if block, which is not on target is found
	for (a = vram; a < vram + SCREEN_TILES_H * SCREEN_TILES_V; a++) {
		if (*a == FONT_SOKOBAN_BLOCK) {
			return 0;
		}
	}

	return 1;
}

/**
 * Gives game into state where was before last push
 */
void sokoban_undo_last_push() {
	// only if there are entries in ringbuffer
	if (sokoban_undo_ringbuffer_count > 0) {
		// pointer to tile where sokoban was before push
		unsigned char* a;
		// pointer to tile where box was before push
		unsigned char* b;
		// pointer to tile where box is now
		unsigned char* c;
		// pointer to tile where sokoban is now
		unsigned char* d;

		// moving back in ringbuffer
		if (sokoban_undo_ringbuffer_pos > 0) {
			sokoban_undo_ringbuffer_pos--;
		} else {
			sokoban_undo_ringbuffer_pos = SOKOBAN_UNDO_RINGBUFFER_SIZE - 1;
		}
		sokoban_undo_ringbuffer_count--;

		// undoing move in vram (level data)
		a = vram + sokoban_undo_ringbuffer[sokoban_undo_ringbuffer_pos].pos;
		b = a + sokoban_undo_ringbuffer[sokoban_undo_ringbuffer_pos].offset;
		c = b + sokoban_undo_ringbuffer[sokoban_undo_ringbuffer_pos].offset;
		d = sokoban_find_player();

		(*d) -= 6;
		(*a) += 6;
		(*c) -= 2;
		(*b) += 2;

		sokoban_moves_level++;
		sokoban_moves++;
		sokoban_pushes_level--;
		sokoban_pushes--;
	} else {
		// if can't get back in ringbuffer, just restart level
		sokoban_load_level();
	}
}

/**
 * Screen to change level
 */
void sokoban_level_select() {
	for (uint8_t i = 0; i < 16*12; i++) {
		vram[i] = FONT_SOKOBAN_SKY;
	}

	uint16_t joypad;

	do {
		uint8_t page = sokoban_level / 100;

		text_pos_x = 1;
		text_pos_y = 0;
		text_print_number(sokoban_level + 1, 3);

		for (uint8_t y = 0; y < 10; y++) {
			text_pos_x = 5 + y;
			text_pos_y = 0;
			text_print_number(y < 9 ? y + 1 : 0, 1);

			text_pos_x = 1;
			text_pos_y = y + 2;
			text_print_number(page * 100 + y * 10, 3);
			for (uint8_t x = 0; x < 10; x++) {
				if (page * 100 + y * 10 + x < SOKOBAN_LEVELS_COUNT) {
					if (page * 100 + y * 10 + x == sokoban_level) {
						vram[(y + 2) * SCREEN_TILES_H + x + 5] = (eeprom_is_level_done(page * 100 + y * 10 + x)) ? FONT_SOKOBAN_TICK_YES_SELECTED : FONT_SOKOBAN_TICK_NO_SELECTED;
					} else {
						vram[(y + 2) * SCREEN_TILES_H + x + 5] = (eeprom_is_level_done(page * 100 + y * 10 + x)) ? FONT_SOKOBAN_TICK_YES : FONT_SOKOBAN_TICK_NO;
					}
				} else {
					vram[(y + 2) * SCREEN_TILES_H + x + 5] = FONT_SOKOBAN_SKY;
				}
			}
		}

		do {
			joypad = ReadJoypad(0);
		} while (joypad);
		do {
			joypad = ReadJoypad(0);
		} while (!joypad);

		if (joypad & BTN_UP) {
			if (sokoban_level >= 10) {
				TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
				sokoban_level -= 10;
			} else {
				TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
			}
		}
		if (joypad & BTN_DOWN) {
			if (sokoban_level < SOKOBAN_LEVELS_COUNT - 10) {
				TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
				sokoban_level += 10;
			} else {
				TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
			}
		}
		if (joypad & BTN_LEFT) {
			if (sokoban_level >= 1) {
				TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
				sokoban_level -= 1;
			} else {
				TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
			}
		}
		if (joypad & BTN_RIGHT) {
			if (sokoban_level < SOKOBAN_LEVELS_COUNT - 1) {
				TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
				sokoban_level += 1;
			} else {
				TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
			}
		}
		if (joypad & BTN_SL) {
			if (sokoban_level >= 100) {
				TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
				sokoban_level -= 100;
			} else {
				TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
			}
		}
		if (joypad & BTN_SR) {
			if (sokoban_level < SOKOBAN_LEVELS_COUNT - 100) {
				TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
				sokoban_level += 100;
			} else {
				TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
			}
		}

	} while (!(joypad & BTN_START) && !(joypad & BTN_A) && !(joypad & BTN_B) && !(joypad & BTN_X) && !(joypad & BTN_Y));
	TriggerFx(SOKOBAN_SOUND_MOVE, 0xff, false);

	// load that level
	SetRenderingParameters(1, 1);
	uint16_t level_to_load = sokoban_level;
	sokoban_restart_game();
	for (uint16_t i = 0; i < level_to_load; i++) {
		sokoban_load_level();
		sokoban_advance_level();
	}
	sokoban_load_level();
	eeprom_set_last_level(sokoban_level);
	SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);
}

/**
 * Process sokoban's move
 * @param int joypad joypad status
 */
inline void sokoban_move(uint16_t joypad) {
	// pointer to tile that sokoban stands on
	unsigned char* a;
	// pointer to tile where sokoban goes to
	unsigned char* b;
	// pointer to tile where sokoban pushes box
	unsigned char* c;

	// pointer offset to next tile (direction of sokoban's move)
	int8_t offset;

	// get offset according to sokoban's direction
	if (joypad & BTN_LEFT) {
		offset = SOKOBAN_OFFSET_LEFT;
	}
	if (joypad & BTN_RIGHT) {
		offset = SOKOBAN_OFFSET_RIGHT;
	}
	if (joypad & BTN_UP) {
		offset = SOKOBAN_OFFSET_UP;
	}
	if (joypad & BTN_DOWN) {
		offset = SOKOBAN_OFFSET_DOWN;
	}

	// button A restarts level
	if (joypad & BTN_A) {
		if (sokoban_moves_level == 0) {
			TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
		} else {
			TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
		}
		sokoban_load_level();
	}

	// button B undoes last push
	if (joypad & BTN_B) {
		if (sokoban_moves_level == 0) {
			TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
		} else {
			TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
		}
		sokoban_undo_last_push();
	}

	// button X advances to next level
	if (joypad & BTN_X) {
		TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
		sokoban_advance_level();
		sokoban_load_level();
		eeprom_set_last_level(sokoban_level);
	}

	// button Y skips to previous level
	if (joypad & BTN_Y) {
		TriggerFx(SOKOBAN_SOUND_KEY, 0xff, false);
		sokoban_skip_to_previous_level();
		eeprom_set_last_level(sokoban_level);
	}

	// button SELECT shows level selection screen
	if (joypad & BTN_SELECT) {
		sokoban_level_select();
	}

	// return if no move should be processed
	if (!(joypad & (BTN_LEFT | BTN_RIGHT | BTN_UP | BTN_DOWN))) {
		return;
	}

	// search for sokoban and next 2 tiles in his way
	a = sokoban_find_player();
	b = a + offset;
	c = b + offset;

	// if sokoban can move that way...
	if (
		((*b >= FONT_SOKOBAN_FLOOR) && (*c >= FONT_SOKOBAN_FLOOR))
		&&
		(
				(*b <= FONT_SOKOBAN_TARGET)
				||
				((*b <= FONT_SOKOBAN_BLOCK_TARGET) && (*c <= FONT_SOKOBAN_TARGET))
		)
	) {
		// ... move him ...
		sokoban_moves++;
		sokoban_moves_level++;
		(*a) -= 6;
		// ... and if must push the block ...
		if (*b >= FONT_SOKOBAN_BLOCK) {
			TriggerFx(SOKOBAN_SOUND_PUSH, 0xff, false);

			// ... record for undo ...
			sokoban_undo_ringbuffer[sokoban_undo_ringbuffer_pos].pos = a - vram;
			sokoban_undo_ringbuffer[sokoban_undo_ringbuffer_pos].offset = offset;
			if (sokoban_undo_ringbuffer_count < SOKOBAN_UNDO_RINGBUFFER_SIZE) {
				sokoban_undo_ringbuffer_count++;
			}
			sokoban_undo_ringbuffer_pos = (sokoban_undo_ringbuffer_pos + 1) % SOKOBAN_UNDO_RINGBUFFER_SIZE;

			// ... and push it
			sokoban_pushes++;
			sokoban_pushes_level++;
			(*b) -= 2;
			(*c) += 2;
		} else {
			TriggerFx(SOKOBAN_SOUND_MOVE, 0xff, false);
		}
		(*b) += 6;
	} else {
		TriggerFx(SOKOBAN_SOUND_CANT_MOVE, 0xff, false);
	}

}

/**
 * Plays "congratulations" music after level was finished
 */
void sokoban_congratulations() {
	TriggerFx(SOKOBAN_SOUND_CONGRAT, 0xff, false);

	WaitVsync(60);
}

/**
 * Sokoban game
 */
void sokoban(void) {
	// joypad status
	uint16_t joypad;

	// start new game
	sokoban_restart_game();

	// load that level
	SetRenderingParameters(1, 1);
	uint16_t last_level = eeprom_get_last_level();
	for (uint16_t i = 0; i < last_level; i++) {
		sokoban_load_level();
		sokoban_advance_level();
	}
	sokoban_load_level();
	SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);

	// main game loop
	do {
		// read joypad status
		do {
			joypad = ReadJoypad(0);
		} while (joypad);
		do {
			joypad = ReadJoypad(0);
		} while (!joypad);

		// move sokoban
		sokoban_move(joypad);
		// refresh game statistics
		sokoban_draw_info();

		// if this move finishes level, skip to next level
		if (sokoban_is_level_cleared()) {
			sokoban_level_done = 1;
			eeprom_set_level_done(sokoban_level);
			sokoban_draw_info();

			sokoban_congratulations();
			sokoban_advance_level();
			sokoban_load_level();
			eeprom_set_last_level(sokoban_level);
		}
	} while (1);
}

/**
 * Draws map, with offset for tiles
 */
void DrawMap2WithTileOffset(unsigned char x,unsigned char y,const char *map, const char offset){
	unsigned char i;
	unsigned char mapWidth=pgm_read_byte(&(map[0]));
	unsigned char mapHeight=pgm_read_byte(&(map[1]));

	for(unsigned char dy=0;dy<mapHeight;dy++){
		for(unsigned char dx=0;dx<mapWidth;dx++){

			i=pgm_read_byte(&(map[(dy*mapWidth)+dx+2]));

			vram[((y+dy)*VRAM_TILES_H)+x+dx]=(i + RAM_TILES_COUNT + offset) ;


		}
	}

}

/**
 * Initializes environment and runs sokoban
 */
int main() {
	SetTileTable(sokoban_tiles);
	InitMusicPlayer(sound_patches);

	DrawMap2WithTileOffset(0, 0, sokoban_font_sokoban_info_1, FONT_SOKOBAN_INFO_OFFSET);
	while (ReadJoypad(0));
	while (!ReadJoypad(0));

	DrawMap2WithTileOffset(0, 0, sokoban_font_sokoban_info_2, FONT_SOKOBAN_INFO_OFFSET);
	while (ReadJoypad(0));
	while (!ReadJoypad(0));

	sokoban();

	for(;;);
}
