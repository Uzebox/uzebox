/*
 *  Uzebox Kernel
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

#include <avr/io.h>
#include <defines.h>

.global InitSoundPort
.global steptable
.global EnableSoundEngine
.global DisableSoundEngine
.global SetMixerNote
.global SetMixerVolume
.global SetMixerWave
.global SetMixerNoiseParams

;Assembly delay macro for 0 to 1275 (old:767) cycles
;Parameters: reg=Registerto use in inner loop (will be destroyed)
;            clocks=CPU clocks to wait
.macro WAIT reg,clocks	
	.if (\clocks) > 767
	 	ldi	\reg, (\clocks)/6    
	 	dec	\reg
		jmp . 
	 	brne .-8
		.if ((\clocks) % 6) == 1
			nop
		.elseif ((\clocks) % 6) == 2
			rjmp .
		.elseif ((\clocks) % 6) == 3
			jmp .
		.elseif ((\clocks) % 6) == 4
			rjmp .
			rjmp .
		.elseif ((\clocks) % 6) == 5
			rjmp .
			jmp .
		.endif
	.else
		.if (\clocks) > 2
		 	ldi	\reg, (\clocks)/3    
		 	dec	\reg                    
		 	brne   .-4
		.endif
		.rept (\clocks) % 3
		 	nop
		.endr
	.endif
.endm 


#if SOUND_MIXER == MIXER_TYPE_INLINE
	#include "soundMixerInline.s"
#else
	#include "soundMixerVsync.s"
#endif	

.section .text

;**********************
; SET NOTE
;(C-call compatible)
; r24: Channel
; r22: -MIDI note, 69=A5(440) for waves channels (0,1,2)
;      -Noise params for channel 3
;***********************
.section .text.SetMixerNote
SetMixerNote:
	clr r25
	clr r23

#if MIXER_CHAN4_TYPE == 0 
	#if SOUND_MIXER == MIXER_TYPE_VSYNC
		cpi r24,3
		brlo set_note_waves
		ret		
	#else
		cpi r24,3
		brne set_note_waves		
		ret
	#endif
#endif

set_note_waves:
	;get step for note
	ldi ZL,lo8(steptable)
	ldi ZH,hi8(steptable)
	lsl r22
	rol r23
	add ZL,r22
	adc ZH,r23	

	lpm r26,Z+
	lpm r27,Z

	ldi ZL,lo8(mixerStruct)
	ldi ZH,hi8(mixerStruct)
	ldi r18,CHANNEL_STRUCT_SIZE
	mul r18,r24
	add ZL,r0
	adc ZH,r1
	
	std Z+step_lo,r26
	std Z+step_hi,r27
	
	clr r1
	

	ret

#if MIXER_CHAN4_TYPE == 0
;**********************
; SET NOISE CHANNEL PARAMS
;(C-call compatible)
; r24: noise divider
;*****************
.section .text.SetMixerNoiseParams
SetMixerNoiseParams:
	;preserve wave type (7/15 bit)
	lds r25,tr4_params
	andi r25,1
	lsl r24
	or r24,r25

	sts tr4_params,r24	
	ret

#endif

;**********************
; SET sound patch for channel 
; (C-call compatible)
; r24 channel
; r23:r22 Waves channels: patch (0x00-0xfd) 
;         Noise channel: 0xfe=7 bit lfsr, 0xff=15 bit lfsr
;                 
;***********************
.section .text.SetMixerWave
SetMixerWave:
	clr r25
	clr r23

#if (INCLUDE_DEFAULT_WAVES != 0)
	ldi ZL,lo8(mixerStruct)
	ldi ZH,hi8(mixerStruct)
	ldi r18,CHANNEL_STRUCT_SIZE
	mul r18,r24	
	add ZL,r0
	adc ZH,r1
#endif

#if MIXER_CHAN4_TYPE == 0
	cpi r22,0xfe	;7bit lfsr
	brne smw1
	lds r22,tr4_params
	andi r22,0xfe;
	sts tr4_params,r22
	rjmp esmw	
smw1:
	cpi r22,0xff	;15bit lfsr
	brne smw2
	lds r22,tr4_params
	ori r22,0xfe;
	sts tr4_params,r22	
	rjmp esmw
smw2:
#endif

#if (INCLUDE_DEFAULT_WAVES != 0)
	ldi r23,hi8(waves)
	add r23,r22
	std Z+samplepos_hi,r23 ;store path No
#endif

esmw:
	clr r1	
	ret




;**********************
; SET Volume
; (C-call compatible)
; r24 channel (0,1,2,3)
; r22 volume (0x00-0xff)
;***********************
.section .text.SetMixerVolume
SetMixerVolume:
	clr r25
	clr r23

	ldi ZL,lo8(mixerStruct)
	ldi ZH,hi8(mixerStruct)
	ldi r18,CHANNEL_STRUCT_SIZE
	mul r18,r24	
	add ZL,r0
	adc ZH,r1
	std Z+vol,r22 ;store vol

	clr r1	
	ret


;*****************************
; Enabled the sound engine (mixed and player).
; C-callable
;*****************************
.section .text.EnableSoundEngine
EnableSoundEngine:	
	ldi r24,1
	sts sound_enabled,r24
	ret

;*****************************
; Disable the sound engine. When disabled, 
; no CPU cycles are consumed.
; C-callable
;*****************************
.section .text.DisableSoundEngine
DisableSoundEngine:	
	ldi r24,0
	sts sound_enabled,r24
	ret

/*
;*****************************
; Initialize the sound port
; by ramping the value from 
; 0 to 0x80 in order to avoid
; an audible "click"
; C-callable
;*****************************
.section .text.InitSoundPort
InitSoundPort:
	clr 24

init_snd_loop:
	
	sts _SFR_MEM_ADDR(OCR2A),r24 ;output sound byte
	inc r24

	;delay to have 1820 cycles 
	;between sound bytes
	ldi r26,lo8(453)
	ldi r27,hi8(453)
	sbiw r26,1
	brne .-4

	cpi r24,0x81
	brlo init_snd_loop
	ret 
*/

.section .text.steptable
steptable:
#include "data/steptable.inc"

#if (INCLUDE_DEFAULT_WAVES != 0)
.section .text.waves
.align 8
waves:
	#include MIXER_WAVES
#endif

.section .text







