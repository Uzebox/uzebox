package org.uzebox.tools.converters.xm;

import java.io.File;
import java.text.DecimalFormat;

import org.apache.commons.io.FileUtils;
import org.apache.log4j.Logger;

public class XmModule {
	static Logger logger = Logger.getLogger(XmModule.class);
	
	//parse Header
	private int headerSize;
	private int songLenght;
	private int restartPosition;
	private int channelsCount;
	private int patternsCount;
	private int instrumentCount;
	private int flags;
	private int tempo;
	private int bpm;	
	
	private int[] orders;
	private Pattern patterns[];
	
	
	public void load(File file){
		byte[] data;
		
		try{
			data=FileUtils.readFileToByteArray(file);
		}catch(Exception e){
			throw new RuntimeException(e);
		}
		
		/* XM header
		   Offset Length Type Ref 					Example
		   ------ ------ ---- ---                   -------	
			0 		17 	char  ID text 				'Extended module: '
			17 		20 	char  Module name 			'Bellissima 99 (mix) '
			37 		1 	byte  0x1A 					1A
			38 		20 	char  Tracker name 			'FastTracker v2.00 '
			58 		2 	word  Version number 		04 01
			60 		4 	dword Header size 			14 01 00 00
			64 		2 	word  Song length 			3E 00 (1..256)
			66 		2 	word  Restart position 		00 00
			68 		2 	word  Number of channels 	20 00 (0..32/64)
			70 		2 	word  Number of patterns 	37 00 (1..256)
			72 		2 	word  Number of instruments 12 00 (0..128)
			74 		2 	word  Flags 				01 00
			76 		2 	word  Default tempo 		05 00
			78 		2 	word  Default BPM 			98 00
			80 		? 	byte  Pattern order table 	00 01 02 03 ...
		*/
		headerSize=parseDWord(data,60);
		songLenght=parseWord(data,64);
		restartPosition=parseWord(data,66);
		channelsCount=parseWord(data,68);
		patternsCount=parseWord(data,70);
		instrumentCount=parseWord(data,72);
		flags=parseWord(data,74);
		tempo=parseWord(data,76);
		bpm=parseWord(data,78);
		
		patterns=new Pattern[patternsCount];
		
		orders=new int[songLenght];
		for(int i=0;i<songLenght;i++){
			orders[i]=(data[80+i])&0xff;
		}

		//debug output 
		if(logger.isDebugEnabled()){
			logger.debug("songLenght: "+songLenght);
			logger.debug("restartPos: "+restartPosition);
			logger.debug("channelsCount: "+channelsCount);
			logger.debug("patternsCount: "+patternsCount);
			logger.debug("instrumentCount: "+instrumentCount);
			logger.debug("flags: "+flags);
			logger.debug("tempo: "+tempo);
			logger.debug("bpm: "+bpm);
			
			StringBuffer tmp=new StringBuffer();
			for(int i=0;i<songLenght;i++){
				if(i>0)tmp.append(",");
				tmp.append(orders[i]);					
			}			
			logger.debug("XM pattern orders: ["+tmp.toString()+"]");
		}
		

		int patPtr=80+songLenght;

		
		//scan all patterns
		for(int pat=0;pat<patternsCount;pat++){
			/*
			Pattern header
			offset size type	desc						example	
				? 	4 	dword 	Pattern header length 		09 00 00 00
				+4 	1 	byte 	Packing type 				00
				+5 	2 	word 	Number of rows in pattern 	20 00 (1..256)
				+7 	2 	word 	Packed pattern data size 	3B 02
				? 	? 			Packed pattern data 		87 3F 01 1A 80 ...
			*/
			int patHeaderLen=parseDWord(data,patPtr);
			int patRows=parseWord(data,patPtr+5);
			int patSize=parseWord(data,patPtr+7);
						
			patterns[pat]=new Pattern(channelsCount,patRows);
				
			
			int ptr=patPtr+patHeaderLen;
			byte b;
			for(int row=0;row<patRows;row++){
				for(int chan=0;chan<channelsCount;chan++){
					b=data[ptr++];
					if((b&0x80)!=0){ //"compression" is used
						if(b!=0x80){ //if row is not empty								
							if((b&0x01)!=0){
								patterns[pat].tracks[chan].getRows()[row].setNote(data[ptr++]);
							}
							if((b&0x02)!=0){
								patterns[pat].tracks[chan].getRows()[row].setInstrument(data[ptr++]);
							}
							if((b&0x04)!=0){
								patterns[pat].tracks[chan].getRows()[row].setVolume(data[ptr++]);
							}

							
							//ignore all effects except "C" (volume) which are converted back to volume column
							//does not override the volume column if set
							byte fx=0,fxParam=0;								
							if((b&0x08)!=0){
								fx=data[ptr++];
							}			
							if((b&0x10)!=0){
								fxParam=data[ptr++];
							}
							if(fx==0xC && !patterns[pat].tracks[chan].getRows()[row].isVolumeSet() ){
								patterns[pat].tracks[chan].getRows()[row].setVolume(fxParam);
							}
						}
					}
				}					
			}
				
				
			
			if(logger.isDebugEnabled() && pat==0){
				DecimalFormat fmt2 = new DecimalFormat("00");
				logger.debug("-------------------------");
				logger.debug("XM Pattern: "+pat);
				logger.debug("patHeaderLen: "+patHeaderLen);
				logger.debug("patRows: "+patRows);
				logger.debug("patSize: "+patSize);
				
			/*
				StringBuffer str=new StringBuffer("\r\n");
				for(int row=0;row<patRows;row++){
					str.append(fmt2.format(row));
					for(int chan=0;chan<channelsCount;chan++){							
						str.append(patterns[pat].tracks[chan].getRows()[row].toString());
					}	
					str.append("\r\n");
				}
				
				logger.debug(str.toString());
				*/
			}
			
										
			patPtr+=(patHeaderLen+patSize);

		}
		
		
	}
	
	private int parseDWord(byte[] data,int offset){
		//int i=0;
		//i=data[offset] + (data[offset+1]<<8) + (data[offset+2]<<16) + (data[offset+3]<<24);		
		//return i;
		return  ((data[offset])&0xff) |
				((data[offset+1]<<8)&0xff00) |
				((data[offset+2]<<16)&0xff0000) |
				((data[offset+3]<<24)&0xff000000);
		
	}
	private int parseWord(byte[] data,int offset){
		
		return ((data[offset])&0xff) |
			   ((data[offset+1]<<8)&0xff00);
		
	}

	public int getSongLenght() {
		return songLenght;
	}

	public void setSongLenght(int songLenght) {
		this.songLenght = songLenght;
	}

	public int getRestartPosition() {
		return restartPosition;
	}

	public void setRestartPosition(int restartPosition) {
		this.restartPosition = restartPosition;
	}

	public int getChannelsCount() {
		return channelsCount;
	}

	public void setChannelsCount(int channelsCount) {
		this.channelsCount = channelsCount;
	}

	public int getPatternsCount() {
		return patternsCount;
	}

	public void setPatternsCount(int patternsCount) {
		this.patternsCount = patternsCount;
	}

	public int getInstrumentCount() {
		return instrumentCount;
	}

	public void setInstrumentCount(int instrumentCount) {
		this.instrumentCount = instrumentCount;
	}

	public int getTempo() {
		return tempo;
	}

	public void setTempo(int tempo) {
		this.tempo = tempo;
	}

	public int getBpm() {
		return bpm;
	}

	public void setBpm(int bpm) {
		this.bpm = bpm;
	}

	public int[] getOrders() {
		return orders;
	}
	public int getOrder(int orderIndex){
		return orders[orderIndex];
	}

	public void setOrders(int[] orders) {
		this.orders = orders;
	}

	public Pattern getPattern(int patternNo){
		return patterns[patternNo]; 
	}
	
	public Pattern getPatternAtOrder(int orderIndex){
		int inOrder=getOrder(orderIndex);		
		return patterns[inOrder]; 
	}
	
	public Pattern[] getPatterns() {
		return patterns;
	}

	public void setPatterns(Pattern[] patterns) {
		this.patterns = patterns;
	}
	
	public XmModule resizePatterns(int oldSize,int newSize){
		
		int multiplier=(oldSize/newSize);
		
		XmModule mod=new XmModule();

		
		Pattern[] outPatterns=new Pattern[patterns.length*multiplier];
		int outPatIndex=0;
		
		//iterate full patterns set
		for(int pat=0;pat<patterns.length;pat++){
			int fullRow=0;
			//cut in chunks
			for(int chunk=0;chunk<multiplier;chunk++){
				outPatterns[outPatIndex]=new Pattern(channelsCount,newSize);				
				//scan each output pattern row
				for(int row=0;row<newSize;row++){
					//scan each channels
					for(int chan=0;chan<channelsCount;chan++){
						TrackPattern.Row r=patterns[pat].tracks[chan].getRows()[fullRow];
						outPatterns[outPatIndex].tracks[chan].getRows()[row]=r;
					}
					fullRow++;
				}
				outPatIndex++;
			}
			
		}
		
		
		DecimalFormat fmt2 = new DecimalFormat("00");
		int pat=0;
		StringBuffer str=new StringBuffer("\r\n");
		for(int row=0;row<patterns[pat].tracks[0].getRows().length;row++){
			str.append(fmt2.format(row));
			for(int chan=0;chan<this.channelsCount;chan++){
				str.append(patterns[pat].tracks[chan].getRows()[row].toString());
			}
			str.append("\r\n");
		}
		str.append("\r\n\r\n");
		
		for(int p=0;p<4;p++){
			for(int row=0;row<outPatterns[p].tracks[0].getRows().length;row++){
				str.append(fmt2.format((16*p)+row));
				for(int chan=0;chan<this.channelsCount;chan++){
					str.append(outPatterns[p].tracks[chan].getRows()[row].toString());
				}
				str.append("\r\n");
			}
		}
		logger.debug(str.toString());
		
		
		
		//expand orders
		int[] outOrders=new int[orders.length*multiplier];
		int outOrderPos=0;
		for(int i=0;i<orders.length;i++){
			for(int j=0;j<multiplier;j++){
				
				outOrders[outOrderPos++]=(orders[i]*multiplier)+j;
			}
			
		}
		
		mod.setBpm(this.bpm);
		mod.setChannelsCount(this.channelsCount);
		mod.setInstrumentCount(instrumentCount);
		mod.setRestartPosition(restartPosition*multiplier);
		mod.setTempo(tempo);
		mod.setSongLenght(outOrders.length);
		mod.setPatterns(outPatterns);
		mod.setOrders(outOrders);
		mod.setPatternsCount(outPatterns.length);
		
		return mod;
	}
	
	
	
}
