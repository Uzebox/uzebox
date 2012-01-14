#include <uzebox.h>
#include <avr/pgmspace.h>
#include <string.h>

#include "atomix.h"
#include "tiles.inc"
#include "levels.h"
#include "sprites.h"
#include "patches.inc"

/**
 * Stores data of playfield
 */
char level_field[LEVEL_FIELD_WIDTH * LEVEL_FIELD_HEIGHT];

/**
 * Stores number of current level (first level has number 1)
 */
uint8_t level = 0; // will be incremented to 1

/**
 * Stores state of cursor
 */
uint8_t cursor_x, cursor_y, cursor_animation;

/**
 * Tile id of holding atom or 0
 */
uint8_t holding_atom;

/**
 * Level statistics
 */
uint32_t moves, distance, time;

/**
 * Data stored in EEPROM
 */
struct EepromBlockStruct eeprom_data;

/**
 * Plays sound (patch) by given id
 * @param unsignd char patch id of patch
 */
void play_sound(unsigned char patch) {
	TriggerFx(patch, 0xFF, true);
}

/**
 * Loads given level
 */
void load_level() {
	memcpy_P(level_field, (level - 1) * LEVEL_BYTE_SIZE + levels + LEVEL_NAME_LENGTH, LEVEL_FIELD_WIDTH * LEVEL_FIELD_HEIGHT);

	cursor_x = 0;
	cursor_y = 0;
	holding_atom = 0;

	time = 0;
	moves = 0;
	distance = 0;
}

/**
 * Draws field
 */
void draw_field() {
	for (uint8_t x = 0; x < LEVEL_FIELD_WIDTH; x++) {
		for (uint8_t y = 0; y < LEVEL_FIELD_HEIGHT; y++) {
			SetTile(SCREEN_FIELD_X + x, SCREEN_FIELD_Y + y, level_field[x + y * LEVEL_FIELD_WIDTH]);
		}
	}
}

/**
 * Draws molecule
 */
void draw_molecule(uint8_t sx, uint8_t sy) {
	for (uint8_t x = 0; x < LEVEL_MOLECULE_WIDTH; x++) {
		for (uint8_t y = 0; y < LEVEL_MOLECULE_HEIGHT; y++) {
			SetTile(sx + x, sy + y, pgm_read_byte(&(levels[(level - 1) * LEVEL_BYTE_SIZE + LEVEL_NAME_LENGTH + LEVEL_FIELD_WIDTH * LEVEL_FIELD_HEIGHT + x + y * LEVEL_MOLECULE_HEIGHT])));
		}
	}
}

/**
 * Prints an uint32_t in decimal to max 8 tiles
 * @param uint8_t x x coordinate on screen
 * @param uint8_t y y coordinate on screen
 * @param uint32_t val value to print
 */
void print_long_8(uint8_t x,uint8_t y, uint32_t val){
	unsigned char c,i;

	for(i=0;i<8;i++){
		c=val%10;
		if(val>0 || i==0){
			PrintChar(x--,y,c+'0');
		}else{
			PrintChar(x--,y,' ');
		}
		val=val/10;
	}
}

/**
 * Prints an uint32_t as time
 * @param uint8_t x x coordinate on screen
 * @param uint8_t y y coordinate on screen
 * @param uint32_t val value to print
 */
void print_time(uint8_t x, uint8_t y, uint32_t val) {
	uint32_t tmp;
	tmp = val;
	tmp /= 60;
	PrintChar(x - 0, y, '0' + (tmp % 10));
	PrintChar(x - 1, y, '0' + ((tmp / 10) % 6));
	tmp /= 60;
	if (tmp > 0) {
		PrintChar(x - 2, y, ':');
		PrintChar(x - 3, y, '0' + (tmp % 10));
		PrintChar(x - 4, y, '0' + ((tmp / 10) % 6));
	}
	tmp /= 60;
	if (tmp > 0) {
		PrintChar(x - 5, y, ':');
		PrintChar(x - 6, y, '0' + (tmp % 10));
		PrintChar(x - 7, y, '0' + ((tmp / 10) % 10));
	}
}

/**
 * Draws statistics of level
 */
void draw_statistics() {
	Print(SCREEN_STATISTICS_X, SCREEN_STATISTICS_Y, PSTR("TIME:"));
	Print(SCREEN_STATISTICS_X, SCREEN_STATISTICS_Y+2, PSTR("MOVES:"));
	Print(SCREEN_STATISTICS_X, SCREEN_STATISTICS_Y+4, PSTR("DIST:"));

	print_time(SCREEN_STATISTICS_X + 7, SCREEN_STATISTICS_Y + 1, time);
	print_long_8(SCREEN_STATISTICS_X + 7, SCREEN_STATISTICS_Y + 3, moves);
	print_long_8(SCREEN_STATISTICS_X + 7, SCREEN_STATISTICS_Y + 5, distance);
}

/**
 * Draws level (molecule) name
 */
void draw_name() {
	Print(SCREEN_TEXT_X, SCREEN_TEXT_Y, PSTR("LEVEL"));
	if (level < 10) {
		PrintChar(SCREEN_TEXT_X + 6, SCREEN_TEXT_Y, '0' + level);
		PrintChar(SCREEN_TEXT_X + 7, SCREEN_TEXT_Y, ':');
	} else {
		PrintChar(SCREEN_TEXT_X + 6, SCREEN_TEXT_Y, '0' + (level / 10));
		PrintChar(SCREEN_TEXT_X + 7, SCREEN_TEXT_Y, '0' + (level % 10));
		PrintChar(SCREEN_TEXT_X + 8, SCREEN_TEXT_Y, ':');
	}
	Print(SCREEN_TEXT_X, SCREEN_TEXT_Y + 2, levels + (level - 1) * LEVEL_BYTE_SIZE);
}

/**
 * Draws arrow at specified point on screen, if field is floor
 */
void draw_arrow(uint8_t x, uint8_t y, uint8_t tile) {
	if (level_field[x + y * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
		SetTile(SCREEN_FIELD_X + x, SCREEN_FIELD_Y + y, tile);
	}
}

/**
 * Hides all sprites
 */
void hide_sprites() {
	for(int i = 0; i < MAX_SPRITES; i++){
		sprites[i].x = (SCREEN_TILES_H * TILE_WIDTH);
	}
}

/**
 * Draws cursor
 */
void draw_cursor() {
	if (holding_atom) {
		sprites[SPRITE_CURSOR].tileIndex = holding_atom;
		draw_arrow(cursor_x, cursor_y - 1, TILE_ARROW_UP);
		draw_arrow(cursor_x + 1, cursor_y, TILE_ARROW_RIGHT);
		draw_arrow(cursor_x, cursor_y + 1, TILE_ARROW_DOWN);
		draw_arrow(cursor_x - 1, cursor_y, TILE_ARROW_LEFT);
	} else {
		cursor_animation = (cursor_animation + 1) % 4;
		sprites[SPRITE_CURSOR].tileIndex = TILE_FIRST_CURSOR + cursor_animation;
		sprites[SPRITE_CURSOR].x = (cursor_x + SCREEN_FIELD_X) * TILE_WIDTH;
		sprites[SPRITE_CURSOR].y = (cursor_y + SCREEN_FIELD_Y) * TILE_HEIGHT;
	}
}

/**
 * Does complete refresh of game screen
 */
void refresh_game_screen() {
	ClearVram();
	DrawMap2(0, 0, tiles_screen);
	draw_field();
	draw_molecule(SCREEN_MOLECULE_X, SCREEN_MOLECULE_Y);
	draw_statistics();
	draw_name();
	draw_cursor();
}

/**
 * Returns, if level is done
 */
uint8_t is_level_done() {
	uint8_t molecule_offset_x = LEVEL_FIELD_WIDTH;
	uint8_t molecule_offset_y = LEVEL_FIELD_HEIGHT;

	for (uint8_t x = 0; x < LEVEL_FIELD_WIDTH; x++) {
		for (uint8_t y = 0; y < LEVEL_FIELD_HEIGHT; y++) {
			if ((level_field[x + y * LEVEL_FIELD_WIDTH] >= TILE_FIRST_ATOM) && (level_field[x + y * LEVEL_FIELD_WIDTH] <= TILE_LAST_ATOM)) {
				if (x < molecule_offset_x) {
					molecule_offset_x = x;
				}
				if (y < molecule_offset_y) {
					molecule_offset_y = y;
				}
			}
		}
	}

	for (uint8_t x = 0; x < LEVEL_MOLECULE_WIDTH; x++) {
		for (uint8_t y = 0; y < LEVEL_MOLECULE_HEIGHT; y++) {
			char molecule_atom = pgm_read_byte(&(levels[(level - 1) * LEVEL_BYTE_SIZE + LEVEL_NAME_LENGTH + LEVEL_FIELD_WIDTH * LEVEL_FIELD_HEIGHT + x + y * LEVEL_MOLECULE_WIDTH]));
			char field_atom = level_field[(x + molecule_offset_x) + (y + molecule_offset_y) * LEVEL_FIELD_WIDTH];

			if ((molecule_atom >= TILE_FIRST_ATOM) && (molecule_atom <= TILE_LAST_ATOM)) {
				if (molecule_atom != field_atom) {
					return false;
				}
			}
		}
	}

	return true;
}

/**
 * Main menu (level selection)
 */
void menu() {
	uint8_t scroll = 0;
	uint8_t delay = false;
	uint8_t e_traj_offset1 = 0;
	uint8_t e_traj_offset2 = ELECTRON_TRAJECTORY_LAST_STEP / 2;

	unsigned int joypad;

	ClearVram();
	hide_sprites();

	for (uint8_t sprite_id = 0; sprite_id < 6; sprite_id++) {
		sprites[sprite_id].tileIndex = TILE_FIRST_ELECTRON + sprite_id;
	}

	DrawMap2(2, 2, logo_logo_map);

	// credits
	Print(2, 23, PSTR("2011 UZEBOX ATOMIX"));
	Print(2, 24, PSTR("PROGRAM: MARTIN SUSTEK"));
	Print(2, 25, PSTR("LEVEL DESIGN: ANDREAS WUEST"));

	// initial scroll position
	scroll = (level - 1);
	if ((level - 1) < SCREEN_MENU_LEVEL_LINES) {
		scroll = 0;
	}
	if ((level - 1) > LEVEL_COUNT - SCREEN_MENU_LEVEL_LINES) {
		scroll = LEVEL_COUNT - SCREEN_MENU_LEVEL_LINES;
	}

	while (true) {
		WaitVsync(1);

		// electrons animation
		e_traj_offset1++;
		if (e_traj_offset1 > ELECTRON_TRAJECTORY_LAST_STEP) {
			e_traj_offset1 = 0;
		}
		e_traj_offset2++;
		if (e_traj_offset2 > ELECTRON_TRAJECTORY_LAST_STEP) {
			e_traj_offset2 = 0;
		}
		sprites[0].x = (SCREEN_ELECTRON_X + pgm_read_byte(e_traj_1x + e_traj_offset1));
		sprites[0].y = (SCREEN_ELECTRON_Y + pgm_read_byte(e_traj_1y + e_traj_offset1));
		sprites[1].x = (SCREEN_ELECTRON_X + pgm_read_byte(e_traj_2x + e_traj_offset2));
		sprites[1].y = (SCREEN_ELECTRON_Y + pgm_read_byte(e_traj_2y + e_traj_offset2));
		sprites[2].x = (SCREEN_ELECTRON_X + pgm_read_byte(e_traj_3x + e_traj_offset1));
		sprites[2].y = (SCREEN_ELECTRON_Y + pgm_read_byte(e_traj_3y + e_traj_offset1));
		sprites[3].x = (SCREEN_ELECTRON_X + pgm_read_byte(e_traj_1x + e_traj_offset2));
		sprites[3].y = (SCREEN_ELECTRON_Y + pgm_read_byte(e_traj_1y + e_traj_offset2));
		sprites[4].x = (SCREEN_ELECTRON_X + pgm_read_byte(e_traj_2x + e_traj_offset1));
		sprites[4].y = (SCREEN_ELECTRON_Y + pgm_read_byte(e_traj_2y + e_traj_offset1));
		sprites[5].x = (SCREEN_ELECTRON_X + pgm_read_byte(e_traj_3x + e_traj_offset2));
		sprites[5].y = (SCREEN_ELECTRON_Y + pgm_read_byte(e_traj_3y + e_traj_offset2));

		for (uint8_t i = 0; i < SCREEN_MENU_LEVEL_LINES; i++) {
			uint8_t current_level = (i + 1 + scroll);
			if ((current_level > 0) && (current_level <= 50)) {
				uint8_t y = 11 + i;
				// level number
				PrintByte(3, y, current_level, false);

				// level name (shorten and pad with spaces)
				for (uint8_t j = 0; j < 24; j++) {
					char ch = pgm_read_byte(levels + (current_level - 1) * LEVEL_BYTE_SIZE + j);
					if (ch == '\0') {
						ch = ' ';
					}
					PrintChar(6 + j, y, ch);
				}

				// level done yet symbol
				if (eeprom_data.data[current_level / 8] & 1 << (current_level % 8)) {
					PrintChar(4, y, FONT_TICK);
				} else {
					PrintChar(4, y, ' ');
				}

				// cursor symbol
				if (current_level == level) {
					PrintChar(5, y, FONT_CURSOR);
				} else {
					PrintChar(5, y, ' ');
				}
			}
		}
		// molecule thumbnail
		draw_molecule(21, 2);

		// controls
		if (delay) {
			WaitVsync(4);
			delay = false;
		}
		joypad = ReadJoypad(0);
		if (joypad & BTN_UP) {
			if (level > 1) {
				play_sound(SOUND_MENU_MOVE);
				level--;
				if (scroll + 1 > level) {
					scroll--;
				}
				delay = true;
			} else {
				play_sound(SOUND_MENU_CANT_MOVE);
			}
		}
		if (joypad & BTN_DOWN) {
			if (level < LEVEL_COUNT) {
				play_sound(SOUND_MENU_MOVE);
				level++;
				if (scroll + SCREEN_MENU_LEVEL_LINES < level) {
					scroll++;
				}
				delay = true;
			} else {
				play_sound(SOUND_MENU_CANT_MOVE);
			}
		}
		if (joypad & BTN_START) {
			hide_sprites();
			play_sound(SOUND_MENU_SELECT);
			return;
		}

	}
}

/**
 * Game loop
 */
void game() {
	while (level < LEVEL_COUNT) {
		level++;
	MENU:
		menu();

		load_level();
		refresh_game_screen();

		while (!is_level_done()) {

			int buttons = ReadJoypad(0);
			char* cursor_field_ptr = &(level_field[cursor_x + cursor_y * LEVEL_FIELD_WIDTH]);

			if (holding_atom) {
				if ((buttons & (BTN_A | BTN_B | BTN_X | BTN_Y))) {
					while (ReadJoypad(0) & (BTN_A | BTN_B | BTN_X | BTN_Y)) {};
					*cursor_field_ptr = holding_atom;
					holding_atom = false;
					draw_field();
					play_sound(SOUND_LEVEL_RELEASE);
				}
				if (buttons & BTN_LEFT) {
					draw_field();
					if (level_field[(cursor_x - 1) + cursor_y * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
						moves++;
						while (level_field[(cursor_x - 1) + cursor_y * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
							play_sound(SOUND_LEVEL_MOVE);
							for (uint8_t i = 0; i < TILE_WIDTH; i++) {
								sprites[SPRITE_CURSOR].x--;
								draw_statistics();
								WaitVsync(1);
							}
							cursor_x--;
							distance++;
						}
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_RIGHT) {
					draw_field();
					if (level_field[(cursor_x + 1) + cursor_y * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
						moves++;
						while (level_field[(cursor_x + 1) + cursor_y * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
							play_sound(SOUND_LEVEL_MOVE);
							for (uint8_t i = 0; i < TILE_WIDTH; i++) {
								sprites[SPRITE_CURSOR].x++;
								draw_statistics();
								WaitVsync(1);
							}
							cursor_x++;
							distance++;
						}
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_UP) {
					draw_field();
					if (level_field[cursor_x + (cursor_y - 1) * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
						moves++;
						while (level_field[cursor_x + (cursor_y - 1) * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
							play_sound(SOUND_LEVEL_MOVE);
							for (uint8_t i = 0; i < TILE_WIDTH; i++) {
								sprites[SPRITE_CURSOR].y--;
								draw_statistics();
								WaitVsync(1);
							}
							cursor_y--;
							distance++;
						}
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_DOWN) {
					draw_field();
					if (level_field[cursor_x + (cursor_y + 1) * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
						moves++;
						while (level_field[cursor_x + (cursor_y + 1) * LEVEL_FIELD_WIDTH] == TILE_FLOOR) {
							play_sound(SOUND_LEVEL_MOVE);
							for (uint8_t i = 0; i < TILE_WIDTH; i++) {
								sprites[SPRITE_CURSOR].y++;
								draw_statistics();
								WaitVsync(1);
							}
							cursor_y++;
							distance++;
						}
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
			} else {
				if ((buttons & (BTN_A | BTN_B | BTN_X | BTN_Y))) {
					if ((*cursor_field_ptr >= TILE_FIRST_ATOM) && (*cursor_field_ptr <= TILE_LAST_ATOM)) {
						while (ReadJoypad(0) & (BTN_A | BTN_B | BTN_X | BTN_Y)) {};
						holding_atom = *cursor_field_ptr;
						*cursor_field_ptr = TILE_FLOOR;
						draw_field();
						play_sound(SOUND_LEVEL_CATCH);
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_LEFT) {
					if (cursor_x > 0) {
						for (uint8_t i = 0; i < TILE_WIDTH; i++) {
							sprites[SPRITE_CURSOR].x--;
							WaitVsync(1);
						}
						cursor_x--;
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_RIGHT) {
					if (cursor_x < LEVEL_FIELD_WIDTH - 1) {
						for (uint8_t i = 0; i < TILE_WIDTH; i++) {
							sprites[SPRITE_CURSOR].x++;
							WaitVsync(1);
						}
						cursor_x++;
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_UP) {
					if (cursor_y > 0) {
						for (uint8_t i = 0; i < TILE_WIDTH; i++) {
							sprites[SPRITE_CURSOR].y--;
							WaitVsync(1);
						}
						cursor_y--;
					} else {
						play_sound(SOUND_LEVEL_CANT_MOVE);
					}
				}
				if (buttons & BTN_DOWN) {
					if (cursor_y < LEVEL_FIELD_HEIGHT - 1) {
						for (uint8_t i = 0; i < TILE_WIDTH; i++) {
							sprites[SPRITE_CURSOR].y++;
							WaitVsync(1);
						}
						cursor_y++;
					} else {
						play_sound(1);
					}
				}
			}
			if (buttons & BTN_SELECT) {
				play_sound(SOUND_MENU_SELECT);
				goto MENU;
			}
			if (buttons & BTN_START) {
				play_sound(SOUND_MENU_SELECT);
				while (ReadJoypad(0) & BTN_START);
				load_level();
				refresh_game_screen();
			}

			draw_cursor();
			draw_statistics();
			WaitVsync(5);
		}

		hide_sprites();
		play_sound(SOUND_LEVEL_CLEARED);
		WaitVsync(60);

		// mark level as cleared
		eeprom_data.data[level / 8] |= 1 << (level % 8);
		EepromWriteBlock(&eeprom_data);
	}
}

/**
 * Vsync callback to measure time
 */
void vsync_callback() {
	time++;
}

/**
 * Main code
 */
int main() {
	SetTileTable(tiles);
	SetSpritesTileTable(tiles);
	SetFontTilesIndex(TILE_FIRST_FONT);
	InitMusicPlayer(patches);
	TriggerNote(3,SOUND_INTRO_MUSIC,20,0xff);
	SetUserPostVsyncCallback(vsync_callback);

	// read custom configuration from eeprom
	EepromReadBlock(129, &eeprom_data);
	eeprom_data.id = 129;

	game();
}
