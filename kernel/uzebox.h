/*
 *  Uzebox Kernel functions
 *  Copyright (C) 2008-2009 Alec Bourque
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
	
	//Include functions & var specific to the video mode used
	#ifdef VMODE_C_PROTOTYPES
		#include VMODE_C_PROTOTYPES
	#endif

	/*
	 * Video Engine structures & functions
	 */
	extern void FadeIn(unsigned char speed,bool blocking);
	extern void FadeOut(unsigned char speed,bool blocking);

	extern void SetSpritesOptions(unsigned char params);
	extern void SetSpritesTileTable(const char *data);
	extern void SetSpriteVisibility(bool visible);
	extern void MapSprite(unsigned char startSprite,const char *map);
	extern void MapSprite2(unsigned char startSprite,const char *map,u8 spriteFlags);
	extern void MoveSprite(unsigned char startSprite,unsigned char x,unsigned char y,unsigned char width,unsigned char height);
	extern void Scroll(char sx,char sy);

	extern void ClearVram(void);
	extern void SetTile(char x,char y, unsigned int tileId);
	extern void SetFont(char x,char y, unsigned char tileId);
	extern void SetFontTilesIndex(unsigned char index);
	extern void SetFontTable(const char *data);
	extern void SetTileTable(const char *data);
	extern void DrawMap(unsigned char x,unsigned char y,const int *map); //draw a map in video mode 1
	extern void DrawMap2(unsigned char x,unsigned char y,const char *map); //draw a map in video mode 2

	extern void Print(int x,int y,const char *string);
	extern void PrintRam(int x,int y,unsigned char *string);
	extern void PrintBinaryByte(char x,char y,unsigned char byte);
	extern void PrintHexByte(char x,char y,unsigned char byte);
	extern void PrintHexInt(char x,char y,int byte);
	extern void PrintHexLong(char x,char y, uint32_t value);
	extern void PrintLong(int x,int y, unsigned long val);
	extern void PrintByte(int x,int y, unsigned char val,bool zeropad);
	extern void PrintChar(int x,int y,char c);
	extern void PrintInt(int x,int y, unsigned int,bool zeropad);

	extern void Fill(int x,int y,int width,int height,int tile);
	extern void FontFill(int x,int y,int width,int height,int tile);

	extern void WaitVsync(int count);
	extern void ClearVsyncFlag(void);
	extern   u8 GetVsyncFlag(void);
	
	extern void SetRenderingParameters(u8 firstScanlineToRender, u8 verticalTilesToRender);


	/*
	 * Sound Engine functions
	 */	
	extern void SetMasterVolume(unsigned char vol);		//global player volume
	extern   u8 GetMasterVolume();
	extern void TriggerNote(unsigned char channel,unsigned char patch,unsigned char note,unsigned char volume);
	extern void TriggerFx(unsigned char patch,unsigned char volume, bool retrig); //uses a simple voice stealing algorithm
	extern void StopSong();
	extern void StartSong(const char *midiSong);
	extern void ResumeSong();
	extern void InitMusicPlayer(const struct PatchStruct *patchPointersParam);
	extern void EnableSoundEngine();
	extern void DisableSoundEngine();


	/*
	 * Controllers functions
	 */
	#define BUTTON_UP 0
	#define BUTTON_DOWN 1
	#define BUTTON_CLICK 2
	#define BUTTON_DBLCLICK 3

	extern unsigned int ReadJoypad(unsigned char joypadNo);
	extern unsigned int ReadJoypadExt(unsigned char joypadNo); //use with SNES mouse
	extern char EnableSnesMouse(unsigned char spriteIndex,const char *spriteMap);
	extern bool SetMouseSensitivity(unsigned char value);
	extern unsigned char GetMouseSensitivity();
	extern unsigned char GetMouseX();	
	extern unsigned char GetMouseY();
	extern unsigned int GetActionButton();
	extern unsigned char DetectControllers();
	void ReadControllers(); //use only if CONTROLLERS_VSYNC_READ=0


	/*
	 * EEPROM functions
	 */
    extern void WriteEeprom(unsigned int addr,unsigned char value);
    extern unsigned char ReadEeprom(unsigned int addr);
	extern char EepromWriteBlock(struct EepromBlockStruct *block);
	extern char EepromReadBlock(unsigned int blockId,struct EepromBlockStruct *block);
	extern bool isEepromFormatted();
	extern void FormatEeprom(void);
	extern void FormatEeprom2(u16 *ids, u8 count);


	/*
	 * UART RX buffer
	 */
	extern void UartInitRxBuffer();
	extern void UartGoBack(unsigned char count);
	extern unsigned char UartUnreadCount();
	extern unsigned char UartReadChar();

	/*
	 * Misc functions
	 */
	extern void WaitUs(unsigned int microseconds);
	extern void SoftReset(void);
	extern bool IsRunningInEmulator(void);

	extern void SetUserPreVsyncCallback(VsyncCallBackFunc);
	extern void SetUserPostVsyncCallback(VsyncCallBackFunc);

#endif
