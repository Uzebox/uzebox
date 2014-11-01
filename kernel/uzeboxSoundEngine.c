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

#include <stdbool.h>
#include <avr/io.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "uzebox.h"

#define CONTROLER_VOL 7
#define CONTROLER_EXPRESSION 11
#define CONTROLER_TREMOLO 92
#define CONTROLER_TREMOLO_RATE 100


#define DEFAULT_WAVE		7
#define DEFAULT_PATCH		0
#define DEFAULT_TRACK_VOL	0xff
#define DEFAULT_EXPRESSION_VOL 0xff

#define MIDI_NULL 0xfd

unsigned int ReadVarLen(const char **songPos);
void SetTriggerCommonValues(struct TrackStruct *track, u8 volume, u8 note);

#if MIDI_IN == 1
	static long received=0;
	bool receivingSysEx=false;
	unsigned char lastMidiInStatus;
#endif 

extern u8 waves[];
extern u16 steptable[];

struct TrackStruct tracks[CHANNELS];

//Common player vars
bool playSong=false;
unsigned char lastStatus;
const char *songPos; 
const char *songStart;
const char *loopStart;
unsigned char masterVolume;
u8 songSpeed;
u8 step;

#if MUSIC_ENGINE == MIDI
	//MIDI player vars
	u16	nextDeltaTime;
	u16	currDeltaTime;
#else
	//MOD players vars
	u8 currentTick;
	u8 currentStep;
	u8 modChannels;
	u8 songLength;
	const u16 *patternOffsets;
	const char *patterns;
#endif



		

/*
 * Command 00: Set envelope speed per frame +127/-128, 0=no enveloppe
 */
void PatchCommand00(struct TrackStruct* track,unsigned char trackNo, char param){
	track->envelopeStep=param;
}
/*
* Command 01: Set noise channel params
*/
void PatchCommand01(struct TrackStruct* track,unsigned char trackNo, char param){
	#if MIXER_CHAN4_TYPE == 0
		mixer.channels.type.noise.barrel=0x0101;
		mixer.channels.type.noise.params=param;
	#endif
}
/*
* Command 02: Set wave
*/
void PatchCommand02(struct TrackStruct* track,unsigned char trackNo, char param){
	SetMixerWave(trackNo,param);
}
/*
* Command 03: Note up * param
*/
void PatchCommand03(struct TrackStruct* track,unsigned char trackNo, char param){
	track->note+=param;
	SetMixerNote(trackNo,track->note);
}
/*
* Command 04: Note down * param
*/
void PatchCommand04(struct TrackStruct* track,unsigned char trackNo, char param){
	track->note-=param;
	SetMixerNote(trackNo,track->note);
}
/*
* Command 05: End of note/fx
*/
void PatchCommand05(struct TrackStruct* track,unsigned char trackNo, char param){
	track->flags&=~(TRACK_FLAGS_PLAYING+TRACK_FLAGS_PRIORITY);	//patchPlaying=false,priority=0	
}

/*
* Command 06: Note hold
*/
void PatchCommand06(struct TrackStruct* track,unsigned char trackNo, char param){
	track->flags|=TRACK_FLAGS_HOLD_ENV; //patchEnvelopeHold=true;
}

/*
* Command 07: Set envelope volume
*/

void PatchCommand07(struct TrackStruct* track,unsigned char trackNo, char param){
	track->envelopeVol=param;
}

/*
* Command 08: Set Note/Pitch
*/

void PatchCommand08(struct TrackStruct* track,unsigned char trackNo, char param){
	SetMixerNote(trackNo,param);
	track->note=param;
	track->flags &= ~(TRACK_FLAGS_SLIDING);	
}

/*
* Command 09: Set tremolo level
*/

void PatchCommand09(struct TrackStruct* track,unsigned char trackNo, char param){
	track->tremoloLevel=param;
}

/*
* Command 10: Set tremolo rate
*/
void PatchCommand10(struct TrackStruct* track,unsigned char trackNo, char param){
	track->tremoloRate=param;
}


/*
* Command 11: Pitch slide (linear), param= (+/-) half steps to slide to
*/

void PatchCommand11(struct TrackStruct* track,unsigned char trackNo, char param){
	//slide to note from current note
	s16 currentStep,targetStep,delta;	
	
	currentStep=pgm_read_word(&(steptable[track->note]));
	targetStep=pgm_read_word(&(steptable[track->note+param]));	
	delta=((targetStep-currentStep)/tracks->slideSpeed);
	if(delta==0)delta++;

	mixer.channels.all[trackNo].step+=delta;
	
	track->slideStep=delta;
	track->flags|=TRACK_FLAGS_SLIDING;
	track->slideNote=track->note+param;
}


/*
* Command 11: Pitch slide speed (fixed 4:4)
*/
void PatchCommand12(struct TrackStruct* track,unsigned char trackNo, char param){
	tracks->slideSpeed=param;
}

const PatchCommand patchCommands[] PROGMEM ={&PatchCommand00,&PatchCommand01,&PatchCommand02,&PatchCommand03,&PatchCommand04,&PatchCommand05,&PatchCommand06,&PatchCommand07,&PatchCommand08,&PatchCommand09,&PatchCommand10,&PatchCommand11,&PatchCommand12};

const struct PatchStruct *patchPointers;

void InitMusicPlayer(const struct PatchStruct *patchPointersParam){

	patchPointers=patchPointersParam;

	masterVolume=DEFAULT_MASTER_VOL;

#if MIDI_IN == ENABLED
	UartInitRxBuffer();
	lastMidiInStatus=0;
#endif

	playSong=false;

	//initialize default channels patches			
	for(unsigned char t=0;t<CHANNELS;t++){		
		tracks[t].flags=TRACK_FLAGS_ALLOCATED;	//allocated=true,priority=0
		tracks[t].noteVol=0;
		tracks[t].expressionVol=DEFAULT_EXPRESSION_VOL;
		tracks[t].trackVol=DEFAULT_TRACK_VOL;
		tracks[t].patchNo=DEFAULT_PATCH;
		tracks[t].tremoloRate=24; //~6hz
		tracks[t].slideSpeed=0x10;
	}

}

#if MUSIC_ENGINE == MIDI

void StartSong(const char *song){
	for(unsigned char t=0;t<CHANNELS;t++){
		tracks[t].flags&=(~TRACK_FLAGS_PRIORITY);// priority=0;
	}

#if MUSIC_ENGINE == MIDI
	songPos=song+1; //skip first delta-time
	songStart=song+1;//skip first delta-time
	loopStart=song+1;
	nextDeltaTime=0;
	currDeltaTime=0;
	songSpeed=0;
#else

	u8 headerSize;
	u8 patternsCount;
	u8 restartPosition;

	//MOD setup
	headerSize=pgm_read_byte(song+0);
	modChannels=pgm_read_byte(song+1);
	patternsCount=pgm_read_byte(song+2);
	step=pgm_read_byte(song+3);
	songSpeed=pgm_read_byte(song+4);
	songLength=pgm_read_byte(song+5);
	restartPosition=pgm_read_byte(song+6);

	songPos=song+headerSize; //poinst to orders
	songStart=song+headerSize; //poinst to orders
	loopStart=song+headerSize+(restartPosition*modChannels);

	patternOffsets=song+headerSize+(songLength*modChannels);
	patterns=song+headerSize+(songLength*modChannels)+(patternsCount*2);

	//TODO: remove
	songPos+=(19*4*modChannels);

	currentTick=0;
	currentStep=0;




#endif

	lastStatus=0;
	playSong=true;

}

#else

	void StartSong(const char *song, u16 startPos, bool loop){
		for(unsigned char t=0;t<CHANNELS;t++){
			tracks[t].flags&=(~TRACK_FLAGS_PRIORITY);// priority=0;
		}

		u8 headerSize;
		u8 patternsCount;
		u8 restartPosition;

		//MOD setup
		headerSize=pgm_read_byte(song+0);
		modChannels=pgm_read_byte(song+1);
		patternsCount=pgm_read_byte(song+2);
		step=pgm_read_byte(song+3);
		songSpeed=pgm_read_byte(song+4);
		songLength=pgm_read_byte(song+5);
		restartPosition=pgm_read_byte(song+6);

		songPos=song+headerSize; //poinst to orders
		songStart=song+headerSize; //poinst to orders
		if(loop){
			loopStart=song+headerSize+(restartPosition*modChannels);
		}else{
			loopStart=NULL;
		}

		patternOffsets=song+headerSize+(songLength*modChannels);
		patterns=song+headerSize+(songLength*modChannels)+(patternsCount*2);

		songPos+=(startPos*modChannels);

		currentTick=0;
		currentStep=0;

		lastStatus=0;
		playSong=true;

	}


#endif




void StopSong(){

	for(u8 i=0;i<CHANNELS;i++){
		tracks[i].envelopeStep=-6;
	}

	playSong=false;
}


void ResumeSong(){
	playSong=true;
}


void SetSongSpeed(u8 speed){
	songSpeed = speed;
}

u8 GetSongSpeed(){
	return songSpeed;
}

#if MIDI_IN == ENABLED

	unsigned char ReadUART(){

		if(UartUnreadCount()!=0){
			received++;
			return UartReadChar();
		}else{
			return MIDI_NULL; //equivalent no NULL
		}
	}

#endif


void ProcessMusic(void){
	u8 c1,c2,tmp,trackVol;
	s16 vol;
	u16 uVol,tVol;
	struct TrackStruct* track;


	//process patches envelopes & pitch slides
	for(unsigned char trackNo=0;trackNo<CHANNELS;trackNo++){
		track=&tracks[trackNo];

		//update envelope
		if(track->envelopeStep!=0){
			vol=track->envelopeVol+track->envelopeStep;		
			if(vol<0){
				vol=0;			
			}else if(vol>0xff){
				vol=0xff;						
			}
			track->envelopeVol=vol;
		}
	
		if(track->flags & TRACK_FLAGS_SLIDING){

			mixer.channels.all[trackNo].step+=track->slideStep;
			u16 tStep=pgm_read_word(&(steptable[track->slideNote]));

			if((track->slideStep>0 && mixer.channels.all[trackNo].step>=tStep) || 
				(track->slideStep<0 && mixer.channels.all[trackNo].step<=tStep))
			{					
				mixer.channels.all[trackNo].step = tStep;					
				track->flags &= ~(TRACK_FLAGS_SLIDING);	
			}
		}
	}



	//Process song MIDI notes
	if(playSong){
	

		#if MUSIC_ENGINE == MIDI
			u8 channel;

			//process all simultaneous events
			while(currDeltaTime==nextDeltaTime){

				c1=pgm_read_byte(songPos++);
			
				if(c1==0xff){
					//META data type event
					c1=pgm_read_byte(songPos++);

				
					if(c1==0x2f){ //end of song
						playSong=false;
						break;	
					}else if(c1==0x6){ //marker
						c1=pgm_read_byte(songPos++); //read len
						c2=pgm_read_byte(songPos++); //read data
						if(c2=='S'){ //loop start
							loopStart=songPos;
						}else if(c2=='E'){//loop end
							songPos=loopStart;
						}
					}
				

				}else{

					if(c1&0x80) lastStatus=c1;					
					channel=lastStatus&0x0f;
				
					//get next data byte
					//Note: maybe we should not advance the cursor
					//in case we receive an unsupported command				
					if(c1&0x80) c1=pgm_read_byte(songPos++); 

					switch(lastStatus&0xf0){

						//note-on
						case 0x90:
							//c1 = note						
							c2=pgm_read_byte(songPos++)<<1; //get volume
						
							if(tracks[channel].flags|TRACK_FLAGS_ALLOCATED){ //allocated==true
								TriggerNote(channel,tracks[channel].patchNo,c1,c2);
							}
							break;

						//controllers
						case 0xb0:
							///c1 = controller #
							c2=pgm_read_byte(songPos++); //get controller value
						
							if(c1==CONTROLER_VOL){
								tracks[channel].trackVol=c2<<1;
							}else if(c1==CONTROLER_EXPRESSION){
								tracks[channel].expressionVol=c2<<1;
							}else if(c1==CONTROLER_TREMOLO){
								tracks[channel].tremoloLevel=c2<<1;
							}else if(c1==CONTROLER_TREMOLO_RATE){
								tracks[channel].tremoloRate=c2<<1;
							}
						
							break;

						//program change
						case 0xc0:
							// c1 = patch #						
							tracks[channel].patchNo=c1;
							break;

					}//end switch(c1&0xf0)


				}//end if(c1==0xff)

				//read next delta time
				nextDeltaTime=ReadVarLen(&songPos);			
				currDeltaTime=0;
		
				#if SONG_SPEED == 1
					if(songSpeed != 0){
						uint32_t l  = (uint32_t)(nextDeltaTime<<8);

						if(songSpeed < 0){//slower
							(uint32_t)(l += (uint32_t)(-songSpeed*(nextDeltaTime<<1)));
							(uint32_t)(l >>= 8);
						}
						else//faster
							(uint32_t)(l /= (uint32_t)((1<<8)+(songSpeed<<1)));

						nextDeltaTime = l;
					}
				#endif

			}//end while
		
			currDeltaTime++;
	
		#else
			
			u8 patternNo,data, note,data2,flags;
			u16 tmp1;

			if(currentTick==0){
				//process next MOD row
				for(u8 trackNo=0;trackNo<modChannels;trackNo++){
					track=&tracks[trackNo];
					const char* patPos;
					
					if(currentStep==0){
						//get pattern order
						patternNo=pgm_read_byte(songPos+trackNo);
						//get pattern address
						tmp1=pgm_read_word(&(patternOffsets[patternNo]));
						patPos=patterns+tmp1;
					}else{
						patPos=track->patternPos;
					}

					data=pgm_read_byte(patPos++);
					note=data&0x7f;
					if(note!=0) track->note=note;

					/*Pack format:
					 *
					 * byte1: [msb,6-0]  msb->inst,vol or fx follows, 6-0: note. 0=no note
					 * byte2: [7-5,4-0] 7-5=001 -> 4-0=instrument
					 *                     =010 -> 4-0=volume
					 *                     =011 -> 4-0=instrument, next byte is volume
					 *                     =100 -> 4-0=FX type, next byte is fx param
					 *                     =101 -> 4-0=instr, next 2 bytes fx type & fx param
					 *                     =110 -> 4-0=vol,  next 2 bytes fx type & fx param
					 *                     =111 -> 4-0=instr, next 3 bytes vol, fx type & fx param
					 */
					if((data&0x80)!=0){
						data2=pgm_read_byte(patPos++);
						flags=data2>>5;
						data2&=0x1f;
						track->noteVol=255; //default vol


						switch(flags){
							case 0x01:
								//instrument byte
								track->patchNo=data2;
								break;
							case 0x02:
								track->noteVol=(data2<<3);
								break;
							case 0x03:
								track->patchNo=data2;
								track->noteVol=(pgm_read_byte(patPos++)<<2);
								break;
							case 0x04:
								patPos+=2; //TODO: skip 2 effects bytes
								break;
							case 0x05:
								track->patchNo=data2;
								patPos+=2; //TODO: skip 2 effects bytes
								break;
							case 0x06:
								track->noteVol=(data2<<2);
								patPos+=2; //TODO: skip 2 effects bytes
								break;
							case 0x07:
								track->patchNo=data2;
								track->noteVol=(pgm_read_byte(patPos++)<<2);
								patPos+=2; //TODO: skip 2 effects bytes
								break;
						
						}
					}
					if(note!=0){
						if(track->flags|TRACK_FLAGS_ALLOCATED){ //allocated==true
							if(trackNo==3){
								TriggerNote(trackNo,0,track->patchNo,track->noteVol); //on noise channel, note actually selects the patch
							}else{
								TriggerNote(trackNo,track->patchNo,note,track->noteVol);
							}
						}
						
					}
					track->patternPos=patPos;										
				}
			}
			currentTick++;
			if(currentTick>songSpeed){
				currentTick=0;
				currentStep++;
				if(currentStep==step){
					currentStep=0;
					songPos+=modChannels;

					//handle loop
					if(songPos>=songStart+(songLength*modChannels)){
						if(loopStart!=NULL){
							songPos=loopStart;
						}else{
							StopSong();
						}
					}

				}
			}

			
	
		#endif


	}//end if(playSong)




	#if MIDI_IN == ENABLED

		// PROCESS MIDI-IN
		//

		bool done=false;

		while(!done){

			c1=ReadUART();
			if(c1==MIDI_NULL)break;


			if(c1<0xf0){//ignore real-time messages

				if(c1&0x80)lastMidiInStatus=c1;					
				channel=c1&0x0f;

				switch(c1&0xf0){

					//note-on
					case 0x90:
						if(UartUnreadCount()<2){
							done=true;
							UartGoBack(1);
						}else{
							c1=ReadUART(); //get note
							c2=ReadUART()<<1; //get volume															
							if(tracks[channel].flags|TRACK_FLAGS_ALLOCATED){
								TriggerNote(channel,tracks[channel].patchNo,c1,c2);
							}
						}
						break;

					//controllers
					case 0xb0:
						if(UartUnreadCount()<2){
							done=true;
							UartGoBack(1);
						}else{
							c1=ReadUART(); //get controller #
							c2=ReadUART(); //get value
							
							if(c1==CONTROLER_VOL){
								tracks[channel].trackVol=c2<<1;
							}else if(c1==CONTROLER_EXPRESSION){
								tracks[channel].expressionVol=c2<<1;
							}else if(c1==CONTROLER_TREMOLO){
								tracks[channel].tremoloLevel=c2<<1;
							}else if(c1==CONTROLER_TREMOLO_RATE){
								tracks[channel].tremoloRate=c2<<1;
							}

						}							
						break;

					//program change
					case 0xc0:
						if(UartUnreadCount()<1){
							done=true;
							UartGoBack(1);
						}else{
							c1=ReadUART(); //get patch
							if(c1==80)c1=8;
							tracks[channel].patchNo=c1;								
						}						
						break;							

					//running status
					default:
						channel=lastMidiInStatus&0x0f;

						switch(lastMidiInStatus&0xf0){

							//note-on
							case 0x90:
								if(UartUnreadCount()<1){
									done=true;
									UartGoBack(1);
								}else{
									c2=ReadUART()<<1; //get volume
									if(tracks[channel].flags|TRACK_FLAGS_ALLOCATED){
										TriggerNote(channel,tracks[channel].patchNo,c1,c2);
									}
								}
								break;

							//controllers
							case 0xb0:
								if(UartUnreadCount()<1){
									done=true;
									UartGoBack(1);
								}else{
									c2=ReadUART(); //get value								
									
									if(c1==CONTROLER_VOL){
										tracks[channel].trackVol=c2<<1;
									}else if(c1==CONTROLER_EXPRESSION){
										tracks[channel].expressionVol=c2<<1;
									}else if(c1==CONTROLER_TREMOLO){
										tracks[channel].tremoloLevel=c2<<1;
									}else if(c1==CONTROLER_TREMOLO_RATE){
										tracks[channel].tremoloRate=c2<<1;
									}
								}
								break;
						
							//program change
							case 0xc0:
								if(c1==80)c1=8;
								tracks[channel].patchNo=c1;
								break;											
						
						}

				}

			}

		}
		
	#endif


	//
	// Process patches command streams & final volume
	//	
	for(unsigned char trackNo=0;trackNo<CHANNELS;trackNo++){
		track=&tracks[trackNo];

		//process patch command stream
		if((track->flags & TRACK_FLAGS_HOLD_ENV)==0){	//patchEnvelopeHold==false

			if(track->patchCommandStreamPos!=NULL && 
				track->patchCurrDeltaTime>=track->patchNextDeltaTime){			

				//process all simultaneous events
				while(track->patchCurrDeltaTime==track->patchNextDeltaTime){	
					
					c1=pgm_read_byte(track->patchCommandStreamPos++);
					if(c1==0xff){					
						//end of stream!
						track->flags&=(~TRACK_FLAGS_PRIORITY);// priority=0;
						track->patchCommandStreamPos=NULL;
						break;

					}else{
						c2=pgm_read_byte(track->patchCommandStreamPos++);
						//invoke patch command function
						( (PatchCommand)pgm_read_word(&patchCommands[c1]) )(track,trackNo,c2);				
					}			
			
					//read next delta time
					track->patchNextDeltaTime=pgm_read_byte(track->patchCommandStreamPos++);						
					
					track->patchCurrDeltaTime=0;	

				}		
			}				
			
			track->patchCurrDeltaTime++;
		}
	


		if(track->flags & TRACK_FLAGS_PLAYING){
		
			if(track->patchPlayingTime<0xff){
				track->patchPlayingTime++;
			}

			//compute final frame volume
			if(track->flags&TRACK_FLAGS_PRIORITY){
				//if an FX, use full track volume.
				trackVol=0xff;
			}else{
				//if regular note, apply MIDI track volume
				trackVol= track->trackVol;
			}
			if(track->noteVol!=0 && track->envelopeVol!=0 && trackVol!=0 && masterVolume!=0){

				uVol=(track->noteVol*trackVol)+0x100;
				uVol>>=8;
				uVol=(uVol*track->envelopeVol)+0x100;
				uVol>>=8;
				uVol=(uVol*track->expressionVol)+0x100;
				uVol>>=8;
				uVol=(uVol*masterVolume)+0x100;
				uVol>>=8;

				if(track->tremoloLevel>0){					
					tmp=pgm_read_byte(&(waves[track->tremoloPos]));
					tmp-=128; //convert to unsigned

					tVol=(track->tremoloLevel*tmp)+0x100;
					tVol>>=8;
					
					uVol=(uVol*(0xff-tVol))+0x100;
					uVol>>=8;
				}

			
			}else{
				uVol=0;
			}	

			track->tremoloPos+=track->tremoloRate;	

		}else{
			uVol=0;
		}
		
		mixer.channels.all[trackNo].volume=(uVol&0xff);
	}
	
}



unsigned int ReadVarLen(const char **songPos)
{
    unsigned int value;
    unsigned char c;


    if ( (value = pgm_read_byte((*songPos)++)) & 0x80 )
    {
       value &= 0x7F;
       do
       {
         value = (value << 7) + ((c = pgm_read_byte((*songPos)++)) & 0x7F);
       } while (c & 0x80);
    }


    return value;
}




void TriggerCommon(u8 channel,u8 patch,u8 volume,u8 note){
	struct TrackStruct* track=&tracks[channel];
		
	bool isFx = (track->flags&TRACK_FLAGS_PRIORITY);

	track->patchCurrDeltaTime=0;
	track->envelopeStep=0; 
	track->envelopeVol=0xff; 
	track->noteVol=volume;
	track->patchPlayingTime=0;
	track->flags|=TRACK_FLAGS_PLAYING;
	track->flags&=(~(TRACK_FLAGS_HOLD_ENV|TRACK_FLAGS_SLIDING));
	track->tremoloLevel=0;
	track->expressionVol=DEFAULT_EXPRESSION_VOL;
	track->note=note;


	#if SOUND_MIXER == MIXER_TYPE_INLINE

		if(channel==3){
			//noise channel		
			if(!isFx) patch=note;			
			mixer.channels.type.noise.barrel=0x0101;				
			mixer.channels.type.noise.params=1; //default (15bits,no divider)

		#if SOUND_CHANNEL_5_ENABLE==1		

		}else if(channel==4){
				//PCM channel					
				mixer.channels.type.pcm.positionFrac=0;
				const char *pos=(const char*)pgm_read_word(&(patchPointers[patch].pcmData));
				mixer.channels.type.pcm.position=pos;				
				mixer.pcmLoopLenght=pgm_read_word(&(patchPointers[patch].loopEnd))-pgm_read_word(&(patchPointers[patch].loopStart));
				mixer.pcmLoopEnd=pos+pgm_read_word(&(patchPointers[patch].loopEnd));
				SetMixerNote(channel,note);
		#endif	

		}else{					
			//wave channels					
			SetMixerWave(channel,0);//default wave
			SetMixerNote(channel,note);
		}		

	#else

		#if MIXER_CHAN4_TYPE == 0
			//if it's a noise channel
			if(channel==3){
				if(!isFx) patch=note;
				mixer.channels.type.noise.barrel=0x0101;				
				mixer.channels.type.noise.params=1; //default 
			}else{
				SetMixerNote(channel,note);
				SetMixerWave(channel,0);
			}
		#else
			//if it's a PCM channel
			if(channel==3){
				mixer.channels.type.pcm.positionFrac=0;
				const char *pos=(const char*)pgm_read_word(&(patchPointers[patch].pcmData));
				mixer.channels.type.pcm.position=pos;
				//mixer.pcmLoopStart=pos+pgm_read_word(&(patchPointers[patch].loopStart));
				mixer.pcmLoopLenght=pgm_read_word(&(patchPointers[patch].loopEnd))-pgm_read_word(&(patchPointers[patch].loopStart));
				mixer.pcmLoopEnd=pos+pgm_read_word(&(patchPointers[patch].loopEnd));
			}else{
				SetMixerWave(channel,0);
			}

			SetMixerNote(channel,note);

		#endif

	#endif //SOUND_MIXER == MIXER_TYPE_INLINE

	if(isFx){
		track->fxPatchNo=patch;
	}else{
		track->patchNo=patch;	
	}

	const char *pos = (const char*)pgm_read_word(&(patchPointers[patch].cmdStream));
	if(pos==NULL){
		track->patchCommandStreamPos=NULL;
	}else{
		track->patchNextDeltaTime=pgm_read_byte(pos++);
		track->patchCommandStreamPos=pos;
	}

}



/* Trigger a sound effect.
 * Method allocates the channel based on priority.
 * Retrig: if this fx if already playing on a track, reuse same track.
 */
void TriggerFx(unsigned char patch,unsigned char volume,bool retrig){
	unsigned char channel;
	
	unsigned char type=(unsigned char)pgm_read_byte(&(patchPointers[patch].type));

	//find the channel to play the fx
	//try to steal voice 2 then 1
	//never steal voice 0, reserve it for lead melodies
	if(type==1 || (type==2 && MIXER_CHAN4_TYPE == 1)){
		//noise or PCM channel fx		
		channel=3;
	}else if(type==2){
		channel=4;
	}else if( (tracks[1].flags&TRACK_FLAGS_PRIORITY)==0 || (tracks[1].fxPatchNo==patch && (tracks[1].flags&TRACK_FLAGS_PRIORITY)!=0 && retrig==true)){ //fx already playing
		channel=1;
	}else if( (tracks[2].flags&TRACK_FLAGS_PRIORITY)==0 || (tracks[2].fxPatchNo==patch && (tracks[2].flags&TRACK_FLAGS_PRIORITY)!=0 && retrig==true)){ //fx already playing				
		channel=2;
	}else{
		//both channels have fx playing, use the oldest one
		if(tracks[1].patchPlayingTime>tracks[2].patchPlayingTime){
			channel=1;
		}else{
			channel=2;
		}
	}				

	tracks[channel].flags|=TRACK_FLAGS_PRIORITY; //priority=1;	
	TriggerCommon(channel,patch,volume,80);
}


void TriggerNote(unsigned char channel,unsigned char patch,unsigned char note,unsigned char volume){

	//allow only other music notes 
	if((tracks[channel].flags&TRACK_FLAGS_PLAYING)==0 || (tracks[channel].flags&TRACK_FLAGS_PRIORITY)==0){
			
		if(volume==0){ //note-off received

			
			//cut note if there's no envelope & no note hold
			if(tracks[channel].envelopeStep==0 && !(tracks[channel].flags&TRACK_FLAGS_HOLD_ENV)){
				tracks[channel].noteVol=0;	
			}

			tracks[channel].flags&=(~TRACK_FLAGS_HOLD_ENV);//patchEnvelopeHold=false;
		}else{
		
			tracks[channel].flags&=(~TRACK_FLAGS_PRIORITY);// priority=0;	
			TriggerCommon(channel,patch,volume,note);
		}

	}
}



void SetMasterVolume(unsigned char vol){
	masterVolume=vol;
}

u8 GetMasterVolume(){
	return masterVolume;
}
