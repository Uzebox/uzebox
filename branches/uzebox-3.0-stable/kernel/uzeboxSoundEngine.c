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

#if MIDI_IN == 1
	static long received=0;
	bool receivingSysEx=false;
	unsigned char lastMidiInStatus;
	unsigned char ramPatch[128];
#else
	unsigned char ramPatch[1];
#endif 

unsigned char ramPatchPtr;

extern unsigned char waves[];

struct TrackStruct tracks[CHANNELS];

//player vars



bool playSong=false;
unsigned int absoluteTime;
int	nextDeltaTime;
int	currDeltaTime;
unsigned char lastStatus;
const char *songPos; 
const char *songStart;
const char *loopStart;
unsigned char masterVolume;




/*
* Command 00: Set envelope speed per frame +127/-128, 0=no enveloppe
*/
void PatchCommand00(unsigned char track, char param){
	tracks[track].envelopeStep=param;
}
/*
* Command 01: Set noise channel params
*/
void PatchCommand01(unsigned char track, char param){

	mixer.channels.type.noise.params=param;
}
/*
* Command 02: Set wave
*/
void PatchCommand02(unsigned char track, char param){
	SetMixerWave(track,param);
}
/*
* Command 03: Note up * param
*/
void PatchCommand03(unsigned char track, char param){
	tracks[track].note+=param;
	SetMixerNote(track,tracks[track].note);
}/*
* Command 04: Note down * param
*/
void PatchCommand04(unsigned char track, char param){
	tracks[track].note-=param;
	SetMixerNote(track,tracks[track].note);
}
/*
* Command 05: End of note/fx
*/
void PatchCommand05(unsigned char track, char param){
	tracks[track].patchPlaying=false;
	tracks[track].priority=0;
}

/*
* Command 06: Note hold
*/
void PatchCommand06(unsigned char track, char param){
	tracks[track].patchEnvelopeHold=true;
}

/*
* Command 07: Set envelope volume
*/

void PatchCommand07(unsigned char track, char param){
	tracks[track].envelopeVol=param;
}

/*
* Command 08: Set Note/Pitch
*/

void PatchCommand08(unsigned char track, char param){
	SetMixerNote(track,param);
	tracks[track].note=param;
}

/*
* Command 09: Set tremolo level
*/

void PatchCommand09(unsigned char track, char param){
	tracks[track].tremoloLevel=param;
}

/*
* Command 10: Set tremolo rate
*/
void PatchCommand10(unsigned char track, char param){
	tracks[track].tremoloRate=param;
}


PatchCommand patchCommands[] PROGMEM ={&PatchCommand00,&PatchCommand01,&PatchCommand02,&PatchCommand03,&PatchCommand04,&PatchCommand05,&PatchCommand06,&PatchCommand07,&PatchCommand08,&PatchCommand09,&PatchCommand10};



//const char **patchPointers; //data in PROGMEM

const struct PatchStruct *patchPointers;

//void InitMusicPlayer(const char *patchPointersParam[]){
void InitMusicPlayer(const struct PatchStruct *patchPointersParam){


	//patchPointers=(const char **)patchPointersParam;
	patchPointers=patchPointersParam;

	masterVolume=DEFAULT_MASTER_VOL;

#if MIDI_IN == ENABLED
	//midi_rx_buf_start=0;
	//midi_rx_buf_end=0;
	UartInitRxBuffer();
	lastMidiInStatus=0;
#endif

	playSong=false;

	//initialize default channels patches			
	for(unsigned char t=0;t<CHANNELS;t++){
		tracks[t].allocated=true;
		tracks[t].noteVol=0;
		tracks[t].expressionVol=DEFAULT_EXPRESSION_VOL;
		tracks[t].trackVol=DEFAULT_TRACK_VOL;
		tracks[t].patchNo=DEFAULT_PATCH;
		tracks[t].priority=0;
		tracks[t].tremoloRate=24; //~6hz
	}

	//tracks[0].tremoloRate=24;
	//tracks[0].tremoloLevel=80;
}

void StartSong(const char *midiSong){
	for(unsigned char t=0;t<CHANNELS;t++){
		tracks[t].priority=0;	
	}

	songPos=midiSong+1; //skip first delta-time
	songStart=midiSong+1;//skip first delta-time
	loopStart=midiSong+1;
	nextDeltaTime=0;
	currDeltaTime=0;
	lastStatus=0;
	playSong=true;
	absoluteTime=0;

	
}

void RestartSong(){	
	StartSong(songStart);
}


void StopSong(){
	//fade out all channels
	if(tracks[0].envelopeStep>=0) tracks[0].envelopeStep=-6;
	if(tracks[1].envelopeStep>=0) tracks[1].envelopeStep=-6;
	if(tracks[2].envelopeStep>=0) tracks[2].envelopeStep=-6;
	if(tracks[3].envelopeStep>=0) tracks[3].envelopeStep=-6;
	playSong=false;
}


void ResumeSong(){
	playSong=true;
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

//static char wait=0;
void ProcessMusic(void){
	unsigned char c1,c2,channel,tmp;
	int vol;
	unsigned int uVol,tVol;	

	

	//process patches envelopes
	for(unsigned char track=0;track<CHANNELS;track++){

		//update envelope
		vol=tracks[track].envelopeVol+tracks[track].envelopeStep;		
		if(vol<0){
			vol=0;			
		}else if(vol>0xff){
			vol=0xff;						
		}
		tracks[track].envelopeVol=vol;
	}



	//Process song MIDI notes
	if(playSong){
	

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
					channel=c1&0x0f;

					switch(c1&0xf0){
	
						//note-on
						case 0x90:
							c1=pgm_read_byte(songPos++); //get note
							c2=pgm_read_byte(songPos++)<<1; //get volume															
							if(tracks[channel].allocated==true){
								TriggerNote(channel,tracks[channel].patchNo,c1,c2);
							}
							break;

						//controllers
						case 0xb0:
							c1=pgm_read_byte(songPos++); //get controller #
							c2=pgm_read_byte(songPos++); //get value
							
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
							c1=pgm_read_byte(songPos++); //get patch
							tracks[channel].patchNo=c1;								
							break;							

						//running status
						default:
							channel=lastStatus&0x0f;

							switch(lastStatus&0xf0){
		
								//note-on
								case 0x90:
									c2=pgm_read_byte(songPos++)<<1; //get volume
									if(tracks[channel].allocated==true){
										TriggerNote(channel,tracks[channel].patchNo,c1,c2);
									}
									break;

								//controllers
								case 0xb0:
									c2=pgm_read_byte(songPos++); //get value	
									
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
									tracks[channel].patchNo=c1;
									break;											
								
							}

					}

				}
				//read next delta time
				nextDeltaTime=ReadVarLen(&songPos); //Bug fix: remove divide by two 			
				currDeltaTime=0;
			
			}//end while

			
			currDeltaTime++;
			absoluteTime++;
	
	}//end if(playSong)


	

	#if MIDI_IN == ENABLED

		// PROCESS MIDI-IN
		//

		bool done=false;

		while(!done){

			c1=ReadUART();
			if(c1==MIDI_NULL)break;


			if(c1==0xf0){ //sysex start
				receivingSysEx=true;
				ramPatchPtr=0;
				//debugPrint(c1);

			}else if(c1==0xf7){	//sysex end	
				receivingSysEx=false;
				//debugPrint(c1);

			}else if(receivingSysEx==true){
				ramPatch[ramPatchPtr++]=c1;	
				//debugPrint(c1);

			}else if(c1<0xf0){//ignore real-time messages

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
							if(tracks[channel].allocated==true){
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
									if(tracks[channel].allocated==true){
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
	
	for(unsigned char track=0;track<CHANNELS;track++){
		//process patch command stream
		if(tracks[track].patchEnvelopeHold==false){

			if(tracks[track].patchCommandStreamPos!=NULL && 
				tracks[track].patchCurrDeltaTime>=tracks[track].patchNextDeltaTime){			

				//process all simultaneous events
				while(tracks[track].patchCurrDeltaTime==tracks[track].patchNextDeltaTime){	
					
					//RAM Patch
					if(tracks[track].patchNo==127){
						c1=*(tracks[track].patchCommandStreamPos++);
						if(c1==0xff){					
							//end of stream!
							tracks[track].priority=0;
							tracks[track].patchCommandStreamPos=NULL;
							break;
						}else{
							c2=*(tracks[track].patchCommandStreamPos++);
							//invoke patch command function
							(patchCommands[c1])(track,c2);				
						}			
				
						//read next delta time
						tracks[track].patchNextDeltaTime=*(tracks[track].patchCommandStreamPos++);
				
					}else{
						//ROM patches
						c1=pgm_read_byte(tracks[track].patchCommandStreamPos++);
						if(c1==0xff){					
							//end of stream!
							tracks[track].priority=0;
							tracks[track].patchCommandStreamPos=NULL;
							break;

						}else{
							c2=pgm_read_byte(tracks[track].patchCommandStreamPos++);
							//invoke patch command function
							( (PatchCommand)pgm_read_word(&patchCommands[c1]) )(track,c2);				
						}			
				
						//read next delta time
						tracks[track].patchNextDeltaTime=pgm_read_byte(tracks[track].patchCommandStreamPos++);
						
					}				
					tracks[track].patchCurrDeltaTime=0;	

				}		
			}				
			
			tracks[track].patchCurrDeltaTime++;
		}
	


		if(tracks[track].patchPlaying){
		
			if(tracks[track].patchPlayingTime<0xff){
				tracks[track].patchPlayingTime++;
			}

			//process final frame volume
			if(tracks[track].noteVol!=0 && tracks[track].envelopeVol!=0 && tracks[track].trackVol!=0 && masterVolume!=0){

				uVol=(tracks[track].noteVol*tracks[track].trackVol)+0x100;
				uVol>>=8;
				uVol=(uVol*tracks[track].envelopeVol)+0x100;
				uVol>>=8;
				uVol=(uVol*tracks[track].expressionVol)+0x100;
				uVol>>=8;
				uVol=(uVol*masterVolume)+0x100;
				uVol>>=8;

				if(tracks[track].tremoloLevel>0){					
					tmp=pgm_read_byte(&(waves[tracks[track].tremoloPos]));
					tmp-=128; //convert to unsigned

					tVol=(tracks[track].tremoloLevel*tmp)+0x100;
					tVol>>=8;
					
					uVol=(uVol*(0xff-tVol))+0x100;
					uVol>>=8;

				}

			
			}else{
				uVol=0;
			}	

			tracks[track].tremoloPos+=tracks[track].tremoloRate;	

		}else{
			uVol=0;
		}
		
		mixer.channels.all[track].volume=(uVol&0xff);
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

/* Trigger a sound effect.
 * Method allocates the channel based on priority.
 * Retrig: if this fx if already playing on a track, reuse same track.
 */
void TriggerFx(unsigned char patch,unsigned char volume,bool retrig){
	unsigned char channel;
	
	const char *pos = (const char*)pgm_read_word(&(patchPointers[patch].cmdStream));
	unsigned char type=(unsigned char)pgm_read_byte(&(patchPointers[patch].type));

	//find the channel to play the fx
	//try to steal voice 2 then 1
	//never steal voice 0, reserve it for lead melodies
	if(type==1 || type==2){
		//noise or PCM channel fx
		channel=3;
	}else if(tracks[1].priority==0 || (tracks[1].fxPatchNo==patch && tracks[1].priority>0 && retrig==true)){ //fx already playing
		channel=1;
	}else if(tracks[2].priority==0 || (tracks[2].fxPatchNo==patch && tracks[2].priority>0 && retrig==true)){ //fx already playing				
		channel=2;
	}else{
		//both channels have fx playing, use the oldest one
		if(tracks[1].patchPlayingTime>tracks[2].patchPlayingTime){
			channel=1;
		}else{
			channel=2;
		}
	}				
	
	

	tracks[channel].patchNextDeltaTime=pgm_read_byte(pos++); //pgm_read_byte(tracks[channel].patchCommandStreamPos++);
	tracks[channel].patchCommandStreamPos=pos;
	tracks[channel].patchCurrDeltaTime=0;		
	tracks[channel].note=80; //default 
	tracks[channel].noteVol=volume;
	tracks[channel].envelopeVol=0xff;
	tracks[channel].envelopeStep=0; 
	tracks[channel].patchEnvelopeHold=false;
	tracks[channel].patchPlayingTime=0;
	tracks[channel].patchPlaying=true;	
	tracks[channel].patchWave=0;
	tracks[channel].fxPatchNo=patch;
	tracks[channel].priority=1;	
	tracks[channel].tremoloLevel=0;
	tracks[channel].expressionVol=DEFAULT_EXPRESSION_VOL;

	#if MIXER_CHAN4_TYPE == 0
		if(channel==3){
			mixer.channels.type.noise.barrel=0x0101;				
			mixer.channels.type.noise.params=1; //default 
		}else{
			SetMixerNote(channel,tracks[channel].note);
			SetMixerWave(channel,tracks[channel].patchWave);
		}
	#else

		if(channel==3){
			mixer.channels.type.wave[3].positionFrac=0;
			const char *pos=(const char*)pgm_read_word(&(patchPointers[patch].pcmData));
			mixer.channels.type.wave[3].position=pos;
			mixer.channels.type.wave[3].loopStart=pos+pgm_read_word(&(patchPointers[patch].loopStart));
			mixer.channels.type.wave[3].loopEnd=pos+pgm_read_word(&(patchPointers[patch].loopEnd));
		}else{
			SetMixerWave(channel,tracks[channel].patchWave);
		}

		SetMixerNote(channel,tracks[channel].note);
		
	#endif

}


void TriggerNote(unsigned char channel,unsigned char patch,unsigned char note,unsigned char volume){

	//allow only other music notes 
	if(!tracks[channel].patchPlaying || tracks[channel].priority==0){
			
		if(volume==0){ //note-off received
			tracks[channel].patchEnvelopeHold=false;
			
			//cut note if theres not envelope
			if(tracks[channel].envelopeStep==0){
				tracks[channel].noteVol=0;	
			}
		}else{

			#if MIXER_CHAN4_TYPE == 0		
				if(channel==3){
					patch=note;
					mixer.channels.type.noise.barrel=0x0101;				
					mixer.channels.type.noise.params=1; //default (15bits,no divider)

				}else{
					SetMixerWave(channel,0); //default wave
					SetMixerNote(channel,note);
				}
			#else
				//unsigned char type=(unsigned char)pgm_read_byte(&(patchPointers[patch].type));
				//if(type==2){
				if(channel==3){
					mixer.channels.type.wave[3].positionFrac=0;
					const char *pos=(const char*)pgm_read_word(&(patchPointers[patch].pcmData));
					mixer.channels.type.wave[3].position=pos;
					mixer.channels.type.wave[3].loopStart=pos+pgm_read_word(&(patchPointers[patch].loopStart));
					mixer.channels.type.wave[3].loopEnd=pos+pgm_read_word(&(patchPointers[patch].loopEnd));
				}else{
					SetMixerWave(channel,0); //default wave
				}
				
				SetMixerNote(channel,note);
			#endif


			if(patch==127){
 				unsigned char *pos = ramPatch;
				tracks[channel].patchNextDeltaTime=*(pos++);
				tracks[channel].patchCommandStreamPos=(char *)pos;
			}else{
				const char *pos = (const char*)pgm_read_word(&(patchPointers[patch].cmdStream));
				if(pos==NULL){
					tracks[channel].patchCommandStreamPos=NULL;
				}else{
					tracks[channel].patchNextDeltaTime=pgm_read_byte(pos++);
					tracks[channel].patchCommandStreamPos=pos;
				}
			}

			
			
			tracks[channel].patchCurrDeltaTime=0;
			tracks[channel].envelopeStep=0; 
			tracks[channel].patchNo=patch;	
			tracks[channel].priority=0;	
			tracks[channel].envelopeVol=0xff; 
			tracks[channel].noteVol=volume;
			tracks[channel].note=note;
			tracks[channel].patchPlayingTime=0;
			tracks[channel].patchPlaying=true;
			tracks[channel].patchEnvelopeHold=false;
			tracks[channel].patchWave=0;
			tracks[channel].tremoloLevel=0;
			tracks[channel].expressionVol=DEFAULT_EXPRESSION_VOL;
		}

	}
}

void SetMasterVolume(unsigned char vol){
	masterVolume=vol;
}

/*
//Print a byte in hexadecimal
void PrintHexByte3(char x,char y,unsigned char byte){
	unsigned char nibble;

	//hi nibble	
	nibble=(byte>>4);
	if(nibble<=9){
		SetFont(x,y,nibble+1+RAM_TILES_COUNT);
	}else{
		SetFont(x,y,nibble+1+ RAM_TILES_COUNT);
	}

	//lo nibble	
	nibble=(byte&0xf);
	if(nibble<=9){		
		SetFont(x+1,y,nibble+1+ RAM_TILES_COUNT);
	}else{
		SetFont(x+1,y,nibble+1+ RAM_TILES_COUNT);
	}

}
*/
//Called each frame 
void DisplayMixStats(char phase,char line){

	unsigned char total;

	if(phase==0){
		total=6-line;
	}else if(phase==1){
		total=6+(6-line);
	}else if(phase==2){
		total=12+(6-line);
	}else{
		total=18+(252-line);
	}

//	PrintHexByte(27,1,total);

}

