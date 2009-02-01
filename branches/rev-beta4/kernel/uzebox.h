/*
 *  Uzebox Kernel functions
 *  Copyright (C) 2008  Alec Bourque
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Uzebox is a reserved trade mark
*/

#ifndef __UZEBOX_H_
#define __UZEBOX_H_

	#include <stdbool.h>
	#include "defines.h"
	#include "kernel.h"

	/*
	 * Video Engine structures & functions
	 */

	extern void FadeIn(unsigned char speed,bool blocking);
	extern void FadeOut(unsigned char speed,bool blocking);

	extern void SetSpritesOptions(unsigned char params);
	extern void SetSpritesTileTable(const char *data);
	extern void SetSpriteVisibility(bool visible);
	extern void MapSprite(unsigned char startSprite,const char *map);
	extern void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height);


	extern void ClearVram(void);
	extern void SetTile(char x,char y, unsigned int tileId);
	extern void SetFont(char x,char y, unsigned char tileId);
	extern void RestoreTile(char x,char y);
	extern void LoadMap(void);
	extern void SetFontTilesIndex(unsigned char index);
	extern void SetFontTable(const char *data);
	extern void SetTileTable(const char *data);
	extern void DrawMap(unsigned char x,unsigned char y,const int *map); //draw a map in video mode 1
	extern void DrawMap2(unsigned char x,unsigned char y,const char *map); //draw a map in video mode 2
	extern void SetTileMap(const int *data);


	extern void Print(int x,int y,const char *string);
	extern void PrintBinaryByte(char x,char y,unsigned char byte);
	extern void PrintHexByte(char x,char y,unsigned char byte);
	extern void PrintHexInt(char x,char y,int byte);
	extern void PrintLong(int x,int y, unsigned long val);
	extern void PrintByte(int x,int y, unsigned char val,bool zeropad);
	extern void PrintChar(int x,int y,char c);
	extern void PrintInt(int x,int y, unsigned int,bool zeropad);

	extern void Fill(int x,int y,int width,int height,int tile);
	extern void FontFill(int x,int y,int width,int height,int tile);

	extern void WaitVsync(int count);
	extern void ClearVsyncFlag(void);
	extern unsigned char GetVsyncFlag(void);
	


	/*
	 * Controllers functions
	 */
	extern unsigned int ReadJoypad(unsigned char joypadNo);
	extern unsigned int ReadJoypadExt(unsigned char joypadNo); //use with SNES mouse
	extern void EnableSnesMouse();
	extern bool SetMouseSensitivity(unsigned char value);
	extern unsigned char GetMouseSensitivity();


	/*
	 * EEPROM functions
	 */
    extern void WriteEeprom(unsigned int addr,unsigned char value);
    extern unsigned char ReadEeprom(unsigned int addr);
	extern char EepromWriteBlock(struct EepromBlockStruct *block);
	extern char EepromReadBlock(unsigned int blockId,struct EepromBlockStruct *block);
	extern bool isEepromFormatted();
	
	/*
	 * Sound Engine functions
	 */	
	extern void SetMasterVolume(unsigned char vol);		//global player volume
	extern void TriggerNote(unsigned char channel,unsigned char patch,unsigned char note,unsigned char volume);
	extern void TriggerFx(unsigned char patch,unsigned char volume, bool retrig); //uses a simple voice stealing algorithm
	extern void StopSong();
	extern void StartSong(const char *midiSong);
	extern void ResumeSong();
	extern void InitMusicPlayer(const struct PatchStruct *patchPointersParam);


	/*
	 * Misc functions
	 */
	extern void WaitUs(unsigned int microseconds);
	extern void SoftReset(void);

#endif
