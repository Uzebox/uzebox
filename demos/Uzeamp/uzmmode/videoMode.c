/*
 *  Uzeamp's video mode (uzmmode): the C side. Mode 5's callbacks (kernel/videoMode5), and the
 *  video's: the sound bank each video frame writes, and the ring's reset.
 */

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "uzebox.h"
#include "intro.h"

#if INTRO_LOGO !=0
	#include "videoMode5/uzeboxlogo_6x8.pic.inc"
	#include "videoMode5/uzeboxlogo_6x8.map.inc"
#endif

extern unsigned char mix_buf[];
extern volatile unsigned char mix_bank;

volatile uint8_t uzm_mute;

//Callback invoked by UzeboxCore.Initialize()
void DisplayLogo(){
	#if INTRO_LOGO !=0
		#define LOGO_X_POS 18
		InitMusicPlayer(logoInitPatches);
		SetTileTable(uzeboxlogo);
		ClearVram();
		WaitVsync(15);
		#if INTRO_LOGO == 1
			TriggerFx(0,0xff,true);
		#endif
		DrawMap(LOGO_X_POS,12,map_uzeboxlogo);
		WaitVsync(6);
		DrawMap(LOGO_X_POS,12,map_uzeboxlogo2);
		WaitVsync(4);
		DrawMap(LOGO_X_POS,12,map_uzeboxlogo);
		#if INTRO_LOGO == 2
			SetMasterVolume(0xc0);
			TriggerNote(3,0,16,0xff);
		#endif
		WaitVsync(64);
		ClearVram();
		WaitVsync(20);
	#endif
}

//Callback invoked by UzeboxCore.Initialize()
void InitializeVideoMode(){
}

/* Called in each vsync, before the kernel flips mix_bank: the bank mix_bank names has just
** been played and stays free through the next field, which the next video frame fills, a
** sector's 7 samples at a time (as Uzeamp's WAV player fills it at once). The other bank, which
** the last frame filled, is played from the flip on: muted, it is silenced first. */
void VideoModeVsync(){
	if (uzm.on) {
		uzm.aw = (uint16_t)(mix_buf + (mix_bank ? MIX_BANK_SIZE : 0));
		if (uzm_mute) {
			memset(mix_buf + (mix_bank ? 0 : MIX_BANK_SIZE), 0x80, MIX_BANK_SIZE);
		}
	}
}

void uzm_reset(void){
	uzm.x  = UZM_RING;
	uzm.y  = UZM_RING;
	uzm.pc = 0;
	uzm.cc = 0;
}

extern volatile unsigned char sync_phase;

/* The kernel starts a frame on the line where sync_pulse enters the window that
** first_render_line and render_lines_count make. Changed right after a frame, the new window
** can hold the line the kernel is on: a frame would start there, end in the middle of the
** next, and the field would never reach its vsync. During the vsync's pulses (phase 0) no frame
** can start, and the next one starts on the new window's first line. */
void uzm_video(uint8_t on){
	while (sync_phase & 1) { }
	if (on) {
		/* this vsync's VideoModeVsync came before uzm.on, and mix_bank has flipped since:
		** the bank it would have named is the other one */
		uzm.aw = (uint16_t)(mix_buf + (mix_bank ? 0 : MIX_BANK_SIZE));
		SetRenderingParameters(UZM_FIRST_LINE, UZM_LINES);
		uzm.on = 1;
	} else {
		uzm.on = 0;
		SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);
	}
}
