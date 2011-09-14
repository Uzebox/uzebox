/*
 * atomix.h
 *
 *  Created on: 27.8.2011
 *      Author: martin
 */

#ifndef ATOMIX_H_
#define ATOMIX_H_

#define TILE_WIDTH 8
#define TILE_HEIGHT 8

#define TILE_EMPTY 0
#define TILE_WALL 1
#define TILE_FLOOR 2
#define TILE_FIRST_ATOM 3
#define TILE_LAST_ATOM 69

#define TILE_FIRST_CURSOR (TILE_LAST_ATOM + 1)
#define TILE_LAST_CURSOR (TILE_FIRST_CURSOR + 3)

#define TILE_ARROW_UP (TILE_LAST_CURSOR + 1)
#define TILE_ARROW_RIGHT (TILE_ARROW_UP + 1)
#define TILE_ARROW_DOWN (TILE_ARROW_RIGHT + 1)
#define TILE_ARROW_LEFT (TILE_ARROW_DOWN + 1)

#define TILE_FIRST_LOGO (TILE_ARROW_LEFT + 1 + 1)
#define TILE_LAST_LOGO (TILE_FIRST_LOGO + 79)

#define TILE_FIRST_ELECTRON (TILE_LAST_LOGO + 1)
#define TILE_LAST_ELECTRON (TILE_FIRST_ELECTRON + 5)

#define TILE_FIRST_FONT (TILE_LAST_ELECTRON + 1 + 1)
#define FONT_TICK ('\\')
#define FONT_CURSOR (FONT_TICK + 3)


#define SCREEN_FIELD_X 3
#define SCREEN_FIELD_Y 5
#define SCREEN_MOLECULE_X 19
#define SCREEN_MOLECULE_Y 5
#define SCREEN_TEXT_X 3
#define SCREEN_TEXT_Y 21
#define SCREEN_MENU_LEVEL_LINES 10
#define SCREEN_STATISTICS_X 19
#define SCREEN_STATISTICS_Y 14

#define SCREEN_ELECTRON_X (40)
#define SCREEN_ELECTRON_Y (10)
#define ELECTRON_TRAJECTORY_LAST_STEP (31)
const uint8_t e_traj_1x[] PROGMEM = {28, 31, 32, 33, 34, 34, 35, 35, 36, 36, 35, 34, 34, 33, 32, 30, 27, 24, 22, 21, 20, 20, 19, 19, 18, 19, 19, 20, 20, 21, 22, 24};
const uint8_t e_traj_1y[] PROGMEM = {2, 4, 6, 10, 12, 16, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 57, 56, 52, 48, 45, 41, 37, 33, 29, 25, 23, 19, 15, 10, 6, 3};
const uint8_t e_traj_2x[] PROGMEM = {23, 27, 31, 36, 40, 45, 47, 49, 52, 52, 48, 45, 43, 40, 37, 34, 29, 24, 21, 18, 14, 11, 8, 5, 2, 1, 4, 7, 11, 14, 18, 20};
const uint8_t e_traj_2y[] PROGMEM = {17, 20, 22, 24, 27, 32, 34, 36, 38, 44, 45, 45, 44, 43, 42, 41, 39, 36, 35, 32, 30, 28, 25, 22, 19, 14, 13, 13, 13, 14, 16, 17};
const uint8_t e_traj_3x[] PROGMEM = {34, 31, 27, 23, 19, 15, 12, 7, 3, 1, 4, 5, 8, 13, 17, 19, 22, 26, 29, 32, 36, 40, 44, 47, 52, 52, 50, 48, 46, 44, 41, 37};
const uint8_t e_traj_3y[] PROGMEM = {34, 36, 38, 40, 41, 43, 44, 45, 44, 41, 36, 23, 31, 28, 25, 23, 22, 20, 19, 17, 16, 14, 13, 13, 14, 18, 21, 23, 25, 28, 29, 32};

#define SOUND_MENU_MOVE 0
#define SOUND_MENU_CANT_MOVE 1
#define SOUND_MENU_SELECT 2
#define SOUND_LEVEL_CATCH 3
#define SOUND_LEVEL_RELEASE 4
#define SOUND_LEVEL_MOVE 5
#define SOUND_LEVEL_CANT_MOVE 1
#define SOUND_LEVEL_CLEARED 6
#define SOUND_INTRO_MUSIC 7

#endif /* ATOMIX_H_ */
