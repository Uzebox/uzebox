#include <uzebox.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <string.h>

#include "graphics.inc"
#include "patches.inc"
#include "music.inc"
#include "chess4uzebox.h"

// position of cursor of each player
uint8_t p1cursor, p2cursor;
// position and status of blinking square
uint8_t blinking1, blinking2, blink_status;
// type of player on each side
uint8_t p1mode, p2mode;
// pieces count for detect capture, promotion
uint8_t pieces_count, nonpawn_pieces_count;
// stores, if last move was computer move
uint8_t was_computer_move;
// stores, if current game is demo
uint8_t is_demo;
// counts moves in game
uint16_t move_count;
// for testing wrong user move
char test_c[4];
// game settings
uint8_t settings;
// counter for 50 moves rule
int8_t rule50;
// flag to indicate that user wants to exit game
int8_t exit_game;
// or-ed status of joypad during thinking (to break game)
int break_joypad;

// avr-micro-max chess engine
#include "avr_umax.c"

/**
 * Plays sound for given event
 * @param uint8_t sound_event type of sound event
 */
void play_sound(uint8_t sound_event) {
	if (settings & SETTINGS_SOUND) {
		TriggerFx(sound_event, 0xFF, true);
	}
}

/**
 * Plays given song
 * @param char *song pointer to start of song
 */
void play_song(char *song) {
	if (settings & SETTINGS_MUSIC) {
		StartSong(song);
	}
}

/**
 * Shows main menu
 */
void show_menu() {
	// prepare screen
	SetTileTable(menu);
	SetFontTilesIndex(MENU_FONT_START);
	ClearVram();
	EnableSoundEngine();
	is_demo = 0;

	// menu interaction
	uint8_t menu_cursor = 0;
	uint8_t *p_mode;
	uint8_t editing_setting;
	uint16_t demo_countdown = DEMO_COUNTDOWN_TOP;
	unsigned int buttons = 0;
	do {
		// refresh screen
		WaitVsync(1);
		DrawMap2(0, 0, menu_screen);
		Print(12, 20, PSTR("Black:"));
		Print(12, 21, PSTR("White:"));
		Print(12, 22, PSTR("Sound:"));
		Print(12, 23, PSTR("Music:"));
		Print(12, 24, PSTR("Turbo:"));
		Print(12, 26, PSTR("press START to play"));
		Print(19, 20, players[p2mode]);
		Print(19, 21, players[p1mode]);
		Print(19, 22, yesno[(settings & SETTINGS_SOUND) ? 1 : 0]);
		Print(19, 23, yesno[(settings & SETTINGS_MUSIC) ? 1 : 0]);
		Print(19, 24, yesno[(settings & SETTINGS_TURBO) ? 1 : 0]);
		Print(10, 20 + menu_cursor, PSTR(">"));

		// wait until key release
		if (buttons) {
			demo_countdown = DEMO_COUNTDOWN_TOP;
			while (ReadJoypad(0) | ReadJoypad(1)) {};
		}

		// interact
		buttons = ReadJoypad(0) | ReadJoypad(1);

		if (buttons & BTN_UP) {
			if (menu_cursor > 0) {
				menu_cursor--;
				play_sound(SOUND_MOVE_CURSOR);
			} else {
				play_sound(SOUND_CANT_MOVE_CURSOR);
			}
		}
		if (buttons & BTN_DOWN) {
			if (menu_cursor < 4) {
				menu_cursor++;
				play_sound(SOUND_MOVE_CURSOR);
			} else {
				play_sound(SOUND_CANT_MOVE_CURSOR);
			}
		}
		if (menu_cursor < 2) {
			p_mode = menu_cursor ? (&p1mode) : (&p2mode);
			if (buttons & BTN_LEFT) {
				if ((*p_mode) > 0) {
					(*p_mode)--;
					play_sound(SOUND_MOVE_CURSOR);
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
			if (buttons & BTN_RIGHT) {
				if ((*p_mode) < 5) {
					(*p_mode)++;
					play_sound(SOUND_MOVE_CURSOR);
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
		} else {
			editing_setting = 1 << (menu_cursor - 2);
			if (buttons & BTN_LEFT) {
				if (settings & editing_setting) {
					settings ^= editing_setting;
					play_sound(SOUND_MOVE_CURSOR);
					if (menu_cursor == 3) {
						StopSong();
					}
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
			if (buttons & BTN_RIGHT) {
				if (!(settings & editing_setting)) {
					settings ^= editing_setting;
					play_sound(SOUND_MOVE_CURSOR);
					if (menu_cursor == 3) {
						ResumeSong();
					}
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
		}

		demo_countdown--;

	} while (!(buttons & BTN_START) && demo_countdown);
	if (!demo_countdown) {
		is_demo = true;
		return;
	}
	while ((ReadJoypad(0) | ReadJoypad(1)) & BTN_START) {};
	if ((settings & (SETTINGS_SOUND | SETTINGS_MUSIC))) {
		EnableSoundEngine();
	} else {
		DisableSoundEngine();
	}
	play_sound(SOUND_SELECT);
}

/**
 * Initializes game
 */
void init_game() {
	SetTileTable(game);
	SetFontTilesIndex(GAME_FONT_START);
	ClearVram();
	DrawMap2(0, 0, game_board);

	p1cursor = 0xFF;
	p2cursor = 0xFF;
	if (p1mode < 2) {
		p1cursor = 0x60;
	}
	if (p2mode < 2) {
		p2cursor = 0x10;
	}

	blinking1 = 0xFF;
	blinking2 = 0xFF;
	pieces_count = 32;
	nonpawn_pieces_count = 16;
	was_computer_move = 0;
	move_count = 0;
	rule50 = 100;
	exit_game = 0;

	// init avr_umax
	new_game();
}

/**
 * Lets user select square
 * @param *cursor pointer to cursor (stores selected square)
 * @param unsigned char joypadNo number of scanned joypad
 * @return uint8_t selected square
 */
uint8_t select_square(uint8_t *cursor, unsigned char joypadNo) {
	unsigned int buttons;

	for (;;) {
		// exit game ?
		if ((ReadJoypad(0) | ReadJoypad(1)) & BTN_SELECT) {
			exit_game = 1;
			return 0;
		}

		// read joypad status
		buttons = ReadJoypad(joypadNo);

		// move cursor
		if (buttons & (BTN_LEFT | BTN_RIGHT | BTN_UP | BTN_DOWN)) {
			if (buttons & BTN_LEFT) {
				if (((*cursor) & 0x07) > 0) {
					(*cursor) -= 1;
					play_sound(SOUND_MOVE_CURSOR);
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
			if (buttons & BTN_RIGHT) {
				if (((*cursor) & 0x07) < 0x07) {
					(*cursor) += 1;
					play_sound(SOUND_MOVE_CURSOR);
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
			if (buttons & BTN_UP) {
				if (((*cursor) & 0x70) > 0) {
					(*cursor) -= 16;
					play_sound(SOUND_MOVE_CURSOR);
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
			if (buttons & BTN_DOWN) {
				if (((*cursor) & 0x70) < 0x70) {
					(*cursor) += 16;
					play_sound(SOUND_MOVE_CURSOR);
				} else {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			}
		}
		blinking1 = *cursor;

		// select square
		if (buttons & (BTN_A | BTN_B | BTN_X | BTN_Y | BTN_SL | BTN_SR)) {
			// wait until release and return
			play_sound(SOUND_SELECT);
			while (ReadJoypad(joypadNo) & (BTN_A | BTN_B | BTN_X | BTN_Y | BTN_SL | BTN_SR)) {
				draw_board();
			};
			return *cursor;
		}

		// wait until key release or return
		uint8_t i = 0;
		while ((buttons = ReadJoypad(joypadNo))) {
			draw_board();
			i++;
			if (i > 10) {
				break;
			}
		};
		// refresh view
		draw_board();
	}
}

/**
 * Reads move from joypad and saves it into c global variable as string and into p saves end position
 */
void read_move() {
	uint8_t *cursor;
	uint8_t *mode;
	unsigned char joypadNo;
	uint8_t source, target;

	blinking1 = 0xFF;
	blinking2 = 0xFF;

	// which side to turn and which controller
	if ((k & 16)) {
		cursor = &p1cursor;
		mode = &p1mode;
	} else {
		cursor = &p2cursor;
		mode = &p2mode;
	}

	if (((*mode) > 1) || (is_demo)) { // computer play
		// set computer level
		if (((*mode) == 2) || (is_demo)) {
			T = 0x20;
			ZMAX = 3;
			w[0]=0; w[1]=1; w[2]=1; w[3]=1; w[4]=-1; w[5]=1; w[6]=1; w[7]=1;
		} else if ((*mode) == 3) {
			T = 0x40;
			ZMAX = 4;
			w[0]=0; w[1]=2; w[2]=2; w[3]=7; w[4]=-1; w[5]=8; w[6]=12; w[7]=23;
		} else if ((*mode) == 4) {
			T = 0x80;
			ZMAX = 5;
			w[0]=0; w[1]=2; w[2]=2; w[3]=7; w[4]=-1; w[5]=8; w[6]=12; w[7]=23;
		} else if ((*mode) == 5) {
			T = 0x400;
			ZMAX = 6;
			w[0]=0; w[1]=2; w[2]=2; w[3]=7; w[4]=-1; w[5]=8; w[6]=12; w[7]=23;
		}

		// prepare computer move animation
		draw_board();
		was_computer_move = 1;

		// empty input means auto-pilot
		c[0] = 10;
		c[1] = 0;
		p = c + 2;

	} else { // human play
		// detect which joypad
		joypadNo = (*mode);

		// do user interaction
		do {
			// select source square (only allow click to own piece)
			do {
				source = select_square(cursor, joypadNo);
				if (((b[source] & 0b00011000) != (k^24)) && (!exit_game)) {
					play_sound(SOUND_CANT_MOVE_CURSOR);
				}
			} while (((b[source] & 0b00011000) != (k^24)) && (!exit_game));
			if (exit_game) {
				return;
			}
			blinking2 = source;
			draw_board();

			// select target square
			target = select_square(cursor, joypadNo);
			blinking1 = 0xFF;
			blinking2 = 0xFF;
			draw_board();

		} while ((target == source) && (!exit_game)); // repeat (cancel move)
		if (exit_game) {
			return;
		}

		// set stack level to be able to check move
		ZMAX = 3;

		// convert source and target to move string
		c[0] = 'a' + (source & 0x07);
		c[1] = '8' - ((source & 0x70) >> 4);
		c[2] = 'a' + (target & 0x07);
		c[3] = '8' - ((target & 0x70) >> 4);
		c[4] = 10;
		c[5] = 0;
		p = c + 6;

		// save old status to detect change by engine (wrong move)
		test_c[0] = c[0];
		test_c[1] = c[1];
		test_c[2] = c[2];
		test_c[3] = c[3];

		// promotion (source is pawn and target is top or bottom line)
		if ((((b[source] & 7) == 1) || ((b[source] & 7) == 2)) && (c[3] == '1' || c[3] == '8')) {

			// ask for under-promotion
			uint8_t selected_piece = 0;
			do {
				DrawMap2(BOARD_PROMOTION_X, BOARD_PROMOTION_Y, game_promotion);
				DrawMap2(BOARD_PROMOTION_X + 1, BOARD_PROMOTION_Y + 1, pieces_map[(c[3]=='1')][0][PIECE_TYPE_QUEEN]);
				DrawMap2(BOARD_PROMOTION_X + 1 + BOARD_SQUARE_SIZE_X, BOARD_PROMOTION_Y + 1, pieces_map[(c[3]=='1')][0][PIECE_TYPE_KNIGHT]);
				DrawMap2(BOARD_PROMOTION_X + 1 + BOARD_SQUARE_SIZE_X * 2, BOARD_PROMOTION_Y + 1, pieces_map[(c[3]=='1')][0][PIECE_TYPE_ROOK]);
				DrawMap2(BOARD_PROMOTION_X + 1 + BOARD_SQUARE_SIZE_X * 3, BOARD_PROMOTION_Y + 1, pieces_map[(c[3]=='1')][0][PIECE_TYPE_BISHOP]);
				if (blink_status & 8) {
					DrawMap2(BOARD_PROMOTION_X + 1 + BOARD_SQUARE_SIZE_X * selected_piece, BOARD_PROMOTION_Y + 1, blink_piece_map);
				}
				WaitVsync(1);
				blink_status++;
				if (ReadJoypad(joypadNo) & BTN_LEFT) {
					if (selected_piece > 0) {
						selected_piece--;
						play_sound(SOUND_MOVE_CURSOR);
						while (ReadJoypad(joypadNo) & BTN_LEFT) {};
					} else {
						play_sound(SOUND_CANT_MOVE_CURSOR);
					}
				}
				if (ReadJoypad(joypadNo) & BTN_RIGHT) {
					if (selected_piece < 3) {
						selected_piece++;
						play_sound(SOUND_MOVE_CURSOR);
						while (ReadJoypad(joypadNo) & BTN_RIGHT) {};
					} else {
						play_sound(SOUND_CANT_MOVE_CURSOR);
					}
				}
			} while (!(ReadJoypad(joypadNo) & (BTN_A | BTN_B | BTN_X | BTN_Y | BTN_SL | BTN_SR)));
			play_sound(SOUND_SELECT);
			while(ReadJoypad(joypadNo)) {};

			// save selected promotion as part of command for engine
			switch (selected_piece) {
			case 1:
				c[4] = 'N';
				break;
			case 2:
				c[4] = 'R';
				break;
			case 3:
				c[4] = 'B';
				break;
			case 0:
			default:
				c[4] = 'Q';
				break;
			}
			c[5] = 10;
			c[6] = 0;
		}
	}
}

/**
 * Draws piece of specified type and color on specified position on board
 *
 * @param uint8_t x x-axis coordinate (a-f)
 * @param uint8_t y y-axis coordinate (8-1 !!)
 * @param uint8_t piece_type type of piece
 * @param uint8_t piece_color color of piece
 * @return void
 */
void draw_piece(uint8_t x, uint8_t y, uint8_t piece_type, uint8_t piece_color) {
	char *piece_map;
	uint8_t square_color = ((x - y) % 2) ? SQUARE_COLOR_BLACK : SQUARE_COLOR_WHITE;

	piece_map = (char*)pieces_map[piece_color][square_color][piece_type];
	if (((blinking1 == (y * 16 + x)) || (blinking2 == (y * 16 + x))) && (blink_status & 8)) {
		piece_map = (char*)blink_piece_map;
	}

	DrawMap2(
		(x * BOARD_SQUARE_SIZE_X + BOARD_OFFSET_X),
		(y * BOARD_SQUARE_SIZE_Y + BOARD_OFFSET_Y),
		piece_map
	);
}

/**
 * Draws whole board (from global variable b of avr-micro-max engine)
 * @return void
 */
void draw_board() {
	uint8_t type;
	uint8_t color;
	char data;
	uint8_t pieces_count_now = 0;
	uint8_t nonpawn_pieces_count_now = 0;

	blink_status++;
	WaitVsync(1);

	// for each square
	for (uint8_t y = 0; y < 8; y++) {
		for (uint8_t x = 0; x < 8; x++) {
			// read data in global variable b
			data = b[y * 16 + x];

			// convert data to piece type and color
			switch (data & 7) {
			case 0:
			default:
				type = PIECE_TYPE_NONE;
				break;
			case 1:
			case 2:
				type = PIECE_TYPE_PAWN;
				pieces_count_now++;
				break;
			case 3:
				type = PIECE_TYPE_KNIGHT;
				pieces_count_now++;
				nonpawn_pieces_count_now++;
				break;
			case 4:
				type = PIECE_TYPE_KING;
				pieces_count_now++;
				nonpawn_pieces_count_now++;
				break;
			case 5:
				type = PIECE_TYPE_BISHOP;
				pieces_count_now++;
				nonpawn_pieces_count_now++;
				break;
			case 6:
				type = PIECE_TYPE_ROOK;
				pieces_count_now++;
				nonpawn_pieces_count_now++;
				break;
			case 7:
				type = PIECE_TYPE_QUEEN;
				pieces_count_now++;
				nonpawn_pieces_count_now++;
				break;
			}

			color = (data & 8) ? PIECE_COLOR_WHITE : PIECE_COLOR_BLACK;

			// draw this piece
			draw_piece(x, y, type, color);
		}
	}

	// print "DEMO" info
	if (is_demo) {
		SetTile(32, 26, GAME_FONT_D);
		SetTile(33, 26, GAME_FONT_E);
		SetTile(34, 26, GAME_FONT_M);
		SetTile(35, 26, GAME_FONT_O);
	}

	// capture
	if (pieces_count_now < pieces_count) {
		play_sound(SOUND_CAPTURE);
		rule50 = 100;
	}

	// promotion
	if (nonpawn_pieces_count_now > nonpawn_pieces_count) {
		play_sound(SOUND_PROMOTION);
	}

	// save new counts
	pieces_count = pieces_count_now;
	nonpawn_pieces_count = nonpawn_pieces_count_now;
}

/**
 * Prints current move into game log
 *
 * @return 1 if game has ended, otherwise 0
 */
uint8_t print_log() {
	uint16_t y;

	// don't log zero move
	if (move_count == 0) {
		move_count = 1;
		return 0;
	}

	// user move test at first
	if (!was_computer_move) {
		if ((test_c[0] != c[0]) || (test_c[1] != c[1]) || (test_c[2] != c[2]) || (test_c[3] != c[3])) {
			// wrong move (was overwriten by engine) - play sound and don't write log
			play_sound(SOUND_MOVE_ERROR);
			return 0;
		}
	}

	// increment move number
	move_count++;

	y = move_count;
	if (y > 25) {
		y = 25;
		// scroll log up
		WaitVsync(1);
		for (uint8_t i = 28; i < 40; i++) {
			for (uint8_t j = 2; j < 26; j++) {
				vram[(i + j * 40)] = vram[(i + (j + 1) * 40)];
			}
		}
	}

	// moving pawn resets rule50 counter
	if (((b[(c[2] - 'a') + (('8' - c[3]) << 4)] & 7) == 1) || ((b[(c[2] - 'a') + (('8' - c[3]) << 4)] & 7) == 2)) {
		rule50 = 100;
	}
	rule50--;

	// print move number
	if (!(move_count % 2)) {
		int tmp = (move_count / 2) % 1000;
		SetTile(30, y, GAME_FONT_0 + tmp % 10);
		tmp /= 10;
		if (tmp > 0) {
			SetTile(29, y, GAME_FONT_0 + tmp % 10);
		}
		tmp /= 10;
		if (tmp > 0) {
			SetTile(28, y, GAME_FONT_0 + tmp % 10);
		}
	}

	// print source and destination
	PrintChar(32, y, c[0] - 'a' + GAME_FONT_A);
	PrintChar(33, y, c[1] - '0' + GAME_FONT_0);
	PrintChar(34, y, c[2] - 'a' + GAME_FONT_A);
	PrintChar(35, y, c[3] - '0' + GAME_FONT_0);

	// computer move - do "animation"
	if (was_computer_move) {
		blinking1 = (c[0] - 'a') + (('8' - c[1]) << 4);
		blinking2 = (c[2] - 'a') + (('8' - c[3]) << 4);
		play_sound(SOUND_COMPUTER_MOVE);
		for (uint8_t i = 0; i < 60; i++) {
			draw_board();
			break_joypad |= ReadJoypad(0) | ReadJoypad(1);
		}
		blinking1 = 0xFF;
		blinking2 = 0xFF;
		draw_board();

		// print draw (if any)
		if ((was_computer_move) && (rule50 < 0)) {
			SetTile(37, y, GAME_FONT_HALF);
			SetTile(38, y, GAME_FONT_HALF);
			play_sound(SOUND_DRAW);
			return 1;
		}

		was_computer_move = 0;
	}

	// print promotion (if any)
	if (c[4] == 'Q') {
		SetTile(36, y, GAME_FONT_QUEEN);
	}
	if (c[4] == 'R') {
		SetTile(36, y, GAME_FONT_ROOK);
	}
	if (c[4] == 'B') {
		SetTile(36, y, GAME_FONT_BISHOP);
	}
	if (c[4] == 'N') {
		SetTile(36, y, GAME_FONT_KNIGHT);
	}

	// print check/mate (if any)
	ZMAX = 4;
	k ^= 24;
	// check?
	if (D(-I,I,0,S,S,1) == I) {
		SetTile(37, y, GAME_FONT_CHECK);
		play_sound(SOUND_CHECK);
		k ^= 24;
		K = I;
		// mate?
		if (!(D(-I,I,0,S,S,3)>-I+1)) {
			SetTile(38, y, GAME_FONT_CHECK);
			play_sound(SOUND_MATE);
			return 1;
		}
		k ^= 24;
	} else {
		// stalemate?
		k ^= 24;
		D(-I,I,0,S,S,3);
		k ^= 24;
		if ((c[0] == c[2]) && (c[1] == c[3])) {
			SetTile(37, y, GAME_FONT_HALF);
			SetTile(38, y, GAME_FONT_HALF);
			play_sound(SOUND_DRAW);
			return 1;
		}
	}
	k ^= 24;

	return 0;
}

/**
 * Switches UI into thinking mode
 */
void start_thinking() {
	if ((settings & SETTINGS_TURBO) && (was_computer_move) && (!is_demo)) {
		WaitVsync(1);
		SetRenderingParameters(1, 1);
		WaitVsync(1);
		SetTileTable(thinking);
		DrawMap2(0, 0, thinking_map);
		SetRenderingParameters(110, 16);
	}
}

/**
 * Switches UI back from thinking mode
 */
void stop_thinking() {
	if (settings & SETTINGS_TURBO) {
		WaitVsync(1);
		SetRenderingParameters(1, 1);
		WaitVsync(1);
		SetTileTable(game);
		DrawMap2(0, 0, game_board_top);
		SetRenderingParameters(20, 224);
	}
}

/**
 * Main code
 */
int main() {
	struct EepromBlockStruct eeprom_data;

	// todo original music
	InitMusicPlayer(patches);

	// default configuration
	p1mode = 0;
	p2mode = 2;
	settings = SETTINGS_SOUND | SETTINGS_MUSIC;
	mysrand(0);

	// read custom configuration from eeprom
	if (EepromReadBlock(EEPR_BLOCK_ID, &eeprom_data) == 0) {
		p1mode = (eeprom_data.data[EEPR_PMODE_POS] & 0xF0) >> 4;
		p2mode = (eeprom_data.data[EEPR_PMODE_POS] & 0x0F);
		settings = (eeprom_data.data[EEPR_SETTINGS_POS] & 0x0F);
		mysrand(eeprom_data.data[EEPR_RANDOM1_POS] << 8 | eeprom_data.data[EEPR_RANDOM2_POS]);
	}

	// main loop
	for (;;) {
		//play_song(music_menu);
		show_menu();
		// save changed settings to eeprom
		eeprom_data.id = EEPR_BLOCK_ID;
		eeprom_data.data[EEPR_PMODE_POS] = (p1mode << 4) | p2mode;
		eeprom_data.data[EEPR_SETTINGS_POS] = settings;
		eeprom_data.data[EEPR_RANDOM1_POS] = r & 0xFF00;
		eeprom_data.data[EEPR_RANDOM2_POS] = r & 0x00FF;
		EepromWriteBlock(&eeprom_data);

		if (!is_demo) {
			play_song((char*)music);
		}
		init_game();
		play_game();
		if (!exit_game && !is_demo) {
			while (ReadJoypad(0) | ReadJoypad(1)) {};
			while (!(ReadJoypad(0) | ReadJoypad(1))) {};
		}
		if (is_demo) {
			WaitVsync(240);
		}

		StopSong();
	}
}
