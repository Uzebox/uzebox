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
extern volatile unsigned char *mix_pos;
extern volatile uint8_t sound_stream;
extern volatile unsigned char sync_phase;
extern volatile unsigned char sync_pulse;

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
	/* Before the flip, mix_pos has played from the start of the bank mix_bank does not name (the
	** one the next field plays) as many samples as the vsync has played: one each odd pulse, as
	** sync_pulse counts them down from 18 (each decremented before it is tested), then one each
	** hsync line. How far into the vsync this call comes changes with the work before it, not
	** that. A song's stream can leave mix_pos elsewhere: while its ring is empty the read holds
	** (about a frame as the song starts) and mix_bank flips on. Out of step, a movie's frame (or
	** processPlayer) fills a bank while it is still played: each field's late lines play samples
	** two frames early, or, paused, one stale sample a field (a crackle 60 times a second). Not
	** streaming, it is put back where it belongs; in step, nothing changes (262 samples a field). */
	if (!(sound_stream & 0x80)) {
		uint8_t sreg = SREG;
		cli();
		uint8_t sp = sync_pulse;
		uint16_t played = (sync_phase & 1) ? 9 + (SYNC_HSYNC_PULSES - sp) : (19 - sp) >> 1;
		volatile unsigned char *want = mix_buf + (mix_bank ? 0 : MIX_BANK_SIZE) + played;
		if (want >= mix_buf + MIX_BUF_SIZE) want -= MIX_BUF_SIZE;
		if (mix_pos != want) mix_pos = want;
		SREG = sreg;
	}
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

/* The frame's geometry (full or cut) in uzm; the sample sectors' limits in lines left */
static void uzm_geometry(uint8_t cut){
	uint8_t s0 = cut ? UZM_CUT_SECTOR : 0;
	uzm.cut   = cut;
	uzm.lines = cut ? UZM_CUT_LINES : UZM_LINES;
	uzm.first = cut ? UZM_CUT_FIRST : UZM_FIRST_LINE;
	uzm.khi   = uzm.lines - 6 * (2 - s0) - 5;
	uzm.klo   = uzm.lines - 6 * (34 - s0) - 5;
	if (uzm.toff < 161) uzm.toff = 161;
}

extern volatile unsigned char sync_phase;
extern volatile unsigned char sync_pulse;

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
		if (!uzm.lines) uzm_geometry(0);
		SetRenderingParameters(uzm.first, uzm.lines);
		uzm.on = 1;
	} else {
		uzm.on = 0;
		SetRenderingParameters(FIRST_RENDER_LINE, FRAME_LINES);
	}
}

/* Mode 5's text frames on their first lines only (or all, FRAME_LINES). Drawing them takes
** nearly all the CPU: a long card job (a big movie's cluster chain) runs 10 times faster on
** 8 lines. Switched in the vsync's pulses, as uzm_video. */
void uzm_text_lines(uint8_t lines){
	while (sync_phase & 1) { }
	SetRenderingParameters(FIRST_RENDER_LINE, lines);
}

/* The kernel starts a frame on a line whose sync_pulse (counting down) is in
** [SYNC_HSYNC_PULSES - first - lines, SYNC_HSYNC_PULSES - first): [5, 233) full, [17, 221) cut.
** Right after a full frame the count is below 5, outside the cut's window: the switch is at
** once. Right after a cut frame it is below 17, which the full window holds down to 5: a frame
** would start there and end in the next field. So to the full frame: below 5, or in the vsync's
** pulses, first (12 lines at most). */
void uzm_cut(uint8_t on){
	if (uzm.on && !on && uzm.cut) {
		while ((sync_phase & 1) && sync_pulse >= SYNC_HSYNC_PULSES - UZM_FIRST_LINE - UZM_LINES) { }
	}
	uzm_geometry(on);
	if (uzm.on) SetRenderingParameters(uzm.first, uzm.lines);
}
