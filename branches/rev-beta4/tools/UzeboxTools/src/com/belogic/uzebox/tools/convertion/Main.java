/*
    Uzebox toolset
    Copyright (C) 2008  Alec Bourque

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.    
 */
package com.belogic.uzebox.tools.convertion;

import java.io.File;
import java.io.IOException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiFileFormat;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Sequence;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Track;

import org.apache.commons.io.FileUtils;
import org.apache.log4j.Logger;



/**
 * @author Admin
 *
 */
public class Main {
	static Logger logger = Logger.getLogger(Main.class);
	

	protected static final int CONTROLER_TREMOLO=92;
	protected static final int CONTROLER_TREMOLO_RATE=100;
	protected static final int CONTROLER_VOL=7;
	
	protected static final int TILE_X_POS=6;
	protected static final int TILE_Y_POS=7;
	protected static final int DATA_START_POS=8;
	private static String path="C:\\Documents and Settings\\Admin\\My Documents\\projet\\atmel\\uzebox2\\";
	
	final static byte R16 =0; 
	final static byte R17 =1;
	final static byte R18 =2;
	final static byte R19 =3;
	final static byte R20 =4;
	final static byte R21 =5;
	
	final static int OP_LDI=0xe0;
	
	public static void main (String [] args) throws Exception{

		File f=new File("c:\\temp\\ApplicationLog.log");
		f.delete();
			
		TileMap[] tetrisMaps={new TileMap("main",0,9,40,28),
						new TileMap("title",0,37,33,9),
						
						new TileMap("anim_spark4",0,1,7,4),
						new TileMap("anim_spark3",7,1,7,4),
						new TileMap("anim_spark2",14,1,7,4),
						new TileMap("anim_spark1",21,1,7,4),
						
						new TileMap("anim_tetris1",20,5,5,2),
						new TileMap("anim_tetris2",25,5,5,2),
						new TileMap("anim_tetris3",30,5,5,2),
						
						new TileMap("anim_tspin",28,1,5,1),
						new TileMap("anim_single",33,1,5,1),
						new TileMap("anim_double",28,2,5,1),
						new TileMap("anim_triple",33,2,5,1),
						
						new TileMap("anim_backtoback3",0,5,5,3),
						new TileMap("anim_backtoback2",10,5,5,3),
						new TileMap("anim_backtoback1",15,5,5,3)
						};
		
		TileMap[] uzeboxlogoMaps={
			new TileMap("uzeboxlogo",0,0,6,4),
			new TileMap("uzeboxlogo2",0,4,6,3),
		};
		
		TileMap[] composerMaps={
					new TileMap("title",1,1,24,3)
					};
		
		TileMap[] arkanoidMaps={
				//new TileMap("main",0,1,22,26),
				//new TileMap("frame",0,27,22,3),
				new TileMap("bg",0,1,5,5),
				new TileMap("logo",0,30,22,5),
				new TileMap("bar",21,30,1,5)
		};
		
		TileMap[] colorMaps={
				new TileMap("colorbars",0,0,40,28)
				};
		
		/*
		 * The following commented calls are examples to build tile sets and convert
		 * MIDI files for the uzebox.
		 * 
		 */	
		//doPictureToTiles(path+"tetris\\tetris-tiles.raw","tetrisTiles",40,46,6,8,tetrisMaps,16);
		//doPictureToTiles(path+"gfx\\fonts_full.raw","fontsFull",91,1,6,8,null,8);
		//doPictureToTiles(path+"composer\\composer_6x8.raw","composerTiles",28,6,6,8,composerMaps,16);		
		//doPictureToTiles(path+"gfx\\uzeboxlogo.raw","uzeboxlogo",6,7,6,8,uzeboxlogoMaps,16);
		//doPictureToTiles(path+"gfx\\sprite_test.raw","spr",17,1,6,8,null,16);
		//doPictureToTiles(path+"gfx\\arkanoid-tiles.raw","arkanoid",22,35,6,8,arkanoidMaps,8);	
//		convertSprites(path+"gfx\\arkanoid-sprites.raw","arkanoid_sprites",22,2,6,8,null,8);
		//doPictureToTiles(path+"gfx\\colorbars.raw","colorbars",40,28,6,8,colorMaps,16);
		
	//	convertRawToIncludeFile("uzeboxbeat","PCM sample");
			
		//convertRawToIncludeFile("voice","PCM sample");
	//	convertRawToIncludeFile("hat_tite","PCM sample");
		
		
		//doPictureToTiles(path+"gfx\\cubes.raw","cubes",29,2,6,8,null,8);
		//doPictureToTiles(path+"gfx\\arkanoid-sprites.raw","arkanoid_sprites",22,2,6,8,null,8);
		//convertSprites(path+"gfx\\arkanoid-sprites.raw","arkanoid_sprites",22,2,6,8,null,8);
		
//		doMonoPictureToTilesAsm(path+"gfx\\fonts_8x8.raw","fonts8x8",16,4,8,8,0,0);
	//	doPictureToTilesAsm(path+"gfx\\fonts_8x8_reduced.raw","fonts8x8reduced",44,1,8,8,0,0);
		 
		//convertSong("tetris\\Korobeiniki-3tracks.mid","include\\Korobeiniki-3tracks.inc","song_korobeiniki",60);
		
		convertSong("music\\pumpkin_f1b.mid","data\\pumpkin.inc","song_pumpkin",160);
		
		//convertSong("music\\ghost.mid","include\\ghost.inc","song_ghost",60);
		//convertSong("music\\mario.mid","include\\mario.inc","song_mario",65);
		//convertSong("music\\NSMB.mid","include\\nsmb.inc","song_nsmb",62);		
		//convertSong("music\\testrisnt__0.mid","include\\testrisnt.inc","song_testrisnt",70);
		//convertSong("music\\tetrisnt_fast.mid","include\\testrisnt_fast.inc","song_testrisnt_fast",60);
		//convertSong("music\\ending2.mid","include\\ending.inc","song_ending",64);
		
		/*
		 * Those ones should be used only for kernel stuff. 
		 */
		//convertWave(path+"SoundDriver\\waves\\sample-9.raw",path+"SoundDriver\\waves\\sample-9.inc","Soft Square");
		//doSrollingTable();		
		//doLfsrTable();
		//doFrequencyTable();
		//doReverseConvert();
		//createPatterns();		
		//doPicture("vidtest2");
	}
	
	private static void convertSprites(String inFilename,String outFilename,int horizontalTiles,int verticalTiles,int tileWidth,int tileHeight,TileMap[] maps,int mapPointersSize) throws Exception {
		File picFile=new File(path+"data\\"+outFilename+".pic.inc");
		File mapFile=new File(path+"data\\"+outFilename+".map.inc");
		File inFile=new File(inFilename);
		
		System.out.println("Processing file "+inFile.getName()+"...");
		
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
		StringBuffer str=new StringBuffer("//picture: "+ inFile.getName() +"\r\n");
		str.append("//Horizontal Tiles="+horizontalTiles+"\r\n");
		str.append("//Vertical Tiles="+verticalTiles+"\r\n");
		str.append("//Tile Width="+tileWidth+"\r\n");
		str.append("//Tile Height="+tileHeight+"\r\n");
		
		List uniqueTiles=new LinkedList();
		
		int[] imageTiles=new int[horizontalTiles*verticalTiles];
			            
		int count=0;
		
		for(int v=0;v<verticalTiles;v++){
			for(int h=0;h<horizontalTiles;h++){
				
				byte[] tile=new byte[tileWidth*tileHeight];
				int tileIndex=0;
				for(int th=0;th<tileHeight;th++){
					for(int tw=0;tw<tileWidth;tw++){
						int index=(v*horizontalTiles*tileWidth*tileHeight)+(h*tileWidth)+(th*horizontalTiles*tileWidth)+ tw;
						tile[tileIndex++]=in[index];												
					}				
				}
				
				int refIndex=-1;				
				//check if tile exist
				for(int i=uniqueTiles.size()-1;i>=0;i--){
					byte[] b=(byte[])uniqueTiles.get(i);
					if(Arrays.equals(b,tile)){
						refIndex=i;
						break;
					}
				}
				
				if(refIndex==-1){
					uniqueTiles.add(tile); 
					imageTiles[count]=uniqueTiles.size()-1;
					
					boolean allZero=true;
					for(int i=0;i<tile.length;i++){
						if(tile[i]!=0)allZero=false;
					}
					if(allZero)
						System.out.println("Blank Tile index="+(uniqueTiles.size()-1));
					
				}else{
					imageTiles[count]=refIndex;
				}
				
				count++;
			}						
		}
		
		//build tile files from unique tiles
		str.append("#define "+outFilename.toUpperCase()+"_TILESET_SIZE "+uniqueTiles.size()+"\r\n\r\n");
		str.append("const unsigned int "+outFilename+"_tileset[] PROGMEM ={\r\n");
		int c=0,t=0;
		for(Iterator it=uniqueTiles.iterator();it.hasNext();){
			byte[] tile=(byte[])it.next();
			
			int data,opcode;
			for(int y=0;y<tileHeight;y++){				
				//build AVR code tiles
				
				//pixel 0
				data=tile[(y*tileWidth)+0]&=0xff;
				opcode=(((R16<<4)+(data&0xf))<<8)+ (OP_LDI+((data>>4)&0xf));
				if(c>0)str.append(",");
				str.append(toHex(opcode));

				//pixel 1
				data=tile[(y*tileWidth)+1]&=0xff;
				opcode=(((R17<<4)+(data&0xf))<<8)+ (OP_LDI+((data>>4)&0xf));
				str.append(",");
				str.append(toHex(opcode));
				
				//pixel 2
				data=tile[(y*tileWidth)+2]&=0xff;
				opcode=(((R18<<4)+(data&0xf))<<8)+ (OP_LDI+((data>>4)&0xf));
				str.append(",");
				str.append(toHex(opcode));
				
				//pixel 3
				data=tile[(y*tileWidth)+3]&=0xff;
				opcode=(((R19<<4)+(data&0xf))<<8)+ (OP_LDI+((data>>4)&0xf));
				str.append(",");
				str.append(toHex(opcode));
				
				//pixel 4
				data=tile[(y*tileWidth)+4]&=0xff;
				opcode=(((R20<<4)+(data&0xf))<<8)+ (OP_LDI+((data>>4)&0xf));
				str.append(",");
				str.append(toHex(opcode));
				
				//pixel 5
				data=tile[(y*tileWidth)+5]&=0xff;
				opcode=(((R21<<4)+(data&0xf))<<8)+ (OP_LDI+((data>>4)&0xf));	
				str.append(",");
				str.append(toHex(opcode));
				
				str.append(",");
				str.append("0xfc01"); //movw ZL,r24
				str.append(",");
				str.append("0x0994"); //ijmp
				
				c++;
				str.append("\r\n");
			}
			
			str.append("\t\t //tile:"+t+" \r\n");
			t++;
			
		}
		str.append("};\r\n");

	
		
		FileUtils.writeStringToFile(picFile, str.toString());	
		System.out.println("Writing file "+picFile.getName()+" done!");

		
	    System.out.println("Unique tiles: "+uniqueTiles.size()+", size="+uniqueTiles.size()*6*8+" bytes");
	    int totalSize=0;

	    System.out.println("Total Size="+totalSize+" bytes");
	}
	private static String toHex(int i){
		if(i<=0xf){
			return "0x000"+Integer.toHexString(i);
		}else if(i<=0xff){
			return "0x00"+Integer.toHexString(i);
		}else if(i<=0xfff){
			return "0x0"+Integer.toHexString(i);
		}else{
			return "0x"+Integer.toHexString(i);
		}
	}
	
	private static String toHex8(int i){
		i&=0xff;
		if(i<=0xf){
			return "0x0"+Integer.toHexString(i);
		}else{
			return "0x"+Integer.toHexString(i);
		}
	}
	
	private static void doLfsrTable() throws Exception{
		
		StringBuffer out=new StringBuffer();
		out.append("//LFSR data\r\n\r\n");
		
		int state=0;
		int barrel=1;
		int tap8,tap7,tap6,tap5,tap4,carry;
		
		//for(int i=0;i<256;i++){
		while(true){
			
			if((barrel&1)==1){
				out.append("\r\n.byte 0x7f ; state No="+state+", val="+barrel);
			}else{
				out.append("\r\n.byte 0x80 ; state No="+state+", val="+barrel);
			}
			
			tap8=barrel & 1;
			tap7=(barrel>>1) & 1;
			tap6=(barrel>>2) & 1;
			tap5=(barrel>>3) & 1;
			tap4=(barrel>>4) & 1;
			carry=tap8^tap6^tap5^tap4;
			//carry=tap8^tap7;
			barrel>>=1;
			barrel|=(carry<<7);
			
			if(barrel==1) break;
			state++;
		}
		
		out.append("\r\n.byte 0x80 \r\n"); //apprend state 256
		
		FileUtils.writeStringToFile(new File(path+"lfsr.inc"), out.toString());
		System.out.println("Outputting file: "+path+"lfsr.inc"+"...done");
		System.out.println("States: "+(state+1));
	}
	

	
	/**
	 * Converts a multi track MIDI song to a single track song and remove useless events. 
	 * 
	 * @param filename
	 * @param outFile
	 * @throws Exception
	 */
	private static void convertSong(String filename,String outFile,String variableName,double speedFactor) throws Exception{	
		
		//define the supported commands
		Map <Integer,Command>commands=new HashMap();
		commands.put(new Integer(0x90),new Command("Note On",true));
		commands.put(new Integer(0xb0),new Command("Controller",true));
		commands.put(new Integer(0xc0),new Command("Program Change",true));
		commands.put(new Integer(0xe0),new Command("Pitch Bend",false));

		//define meta events
		Map <Integer,MetaEvent>metaEvents=new HashMap();
		metaEvents.put(new Integer(0x2),new MetaEvent("Copyright",false));
		metaEvents.put(new Integer(0x3),new MetaEvent("Track Name",false));
		metaEvents.put(new Integer(0x6),new MetaEvent("Marker",true));
		metaEvents.put(new Integer(0x2f),new MetaEvent("End of Track",true));
		metaEvents.put(new Integer(0x51),new MetaEvent("Tempo",true));
		metaEvents.put(new Integer(0x58),new MetaEvent("Time Signature",false));
		metaEvents.put(new Integer(0x59),new MetaEvent("Key Signature",false));
		
		//create in/out files
		File inFile=new File(path+filename);
		
		MidiFileFormat format=MidiSystem.getMidiFileFormat(inFile);
		
		if(format.getType()!=0){
			throw new RuntimeException("Unsupported file format "+format.getType()+". Only MIDI file format 0 (all events in one track) is supported.");
		}
		
		Sequence inSequence=MidiSystem.getSequence(inFile);		
		
		Sequence seq=new Sequence(format.getDivisionType(),format.getResolution(),1);
		Track outTrack=seq.getTracks()[0];
		
		long tempo=60000000/format.getResolution();
		int resolution=format.getResolution();
		//if(resolution!=120){
			//throw new RuntimeException("MIDI file resolution must be 120. Currently:"+resolution);
		//}

		Track track=inSequence.getTracks()[0];

		for(int e=0;e<track.size();e++){
			MidiEvent event=track.get(e);				
			
			if(event.getMessage() instanceof MetaMessage){
				MetaMessage m=(MetaMessage)event.getMessage();
				if(metaEvents.containsKey(m.getType()) && metaEvents.get(m.getType()).supported){
					if(m.getType()==6 &&  m.getLength()>4)
						throw new RuntimeException("META markers text size must by only one character: "+new String(m.getData()));
					
					if(m.getType()==0x51){ //tempo
						byte[] t=m.getData();
						long tmp=(t[0]<<16)+(t[1]<<8)+t[2];
						tempo=tmp;
					}else{
						addEvent(e,outTrack,event,tempo,speedFactor);
						logger.debug("META: tick="+event.getTick()+",type="+Integer.toHexString(m.getType())+":"+metaEvents.get(m.getType()).name);
					}
				}else{
					String name="";
					if(metaEvents.get(m.getType())!=null) name=metaEvents.get(m.getType()).name;
					//logger.warn("META: tick="+tick+",type="+Integer.toHexString(m.getType())+":"+name+": UNSUPPORTED");
				}
			}else if(event.getMessage() instanceof ShortMessage){
				ShortMessage m=(ShortMessage)event.getMessage();
				if(m.getChannel()==9){
					ShortMessage newMessage =new ShortMessage();
					newMessage.setMessage(m.getCommand(), 3, m.getData1(),m.getData2());
					event=new MidiEvent(newMessage,event.getTick());	
					m=(ShortMessage)event.getMessage();
				}
				
				
				if(commands.containsKey(m.getCommand()) && commands.get(m.getCommand()).supported){
					if(m.getCommand()==0xb0){ //controllers
						if(m.getData1()==CONTROLER_VOL || m.getData1()==CONTROLER_TREMOLO || m.getData1()==CONTROLER_TREMOLO_RATE){
							addEvent(e,outTrack,event,tempo,speedFactor);
							logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"	+commands.get(m.getCommand()).name+"type="+m.getData1());								
						}else{
							//logger.debug("MIDI:tick="+tick+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"	+commands.get(m.getCommand()).name+"type="+m.getData1()+": UNSUPPORTED");
						}
					}else if(m.getCommand()==0x90 && m.getData2()==0){ //note off
						//logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":Note Off: IGNORED");
					}else{
						addEvent(e,outTrack,event,tempo,speedFactor);
						logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"	+commands.get(m.getCommand()).name);
					}
				}else{
					String name="";
					if(commands.get(m.getCommand())!=null) name=commands.get(m.getCommand()).name;
					//logger.warn("MIDI:tick="+tick+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"+name+": UNSUPPORTED");
				}
			}else{
				//logger.warn("SYSEX: tick="+tick+": UNSUPPORTED");
			}

		}

	
        File temp = File.createTempFile("~mid", ".tmp");        
        temp.deleteOnExit();
        MidiSystem.write(seq, 0, temp);
        
        byte data[]=FileUtils.readFileToByteArray(temp);
        
        //output track data skipping headers
		StringBuffer out=new StringBuffer();
		out.append("//*********************************//\r\n");
		out.append("// MIDI file: "+path+filename+"\r\n");
		out.append("//*********************************//\r\n");

		out.append("const char "+variableName+"[] PROGMEM ={\r\n");
		int totalSize=0; 
		
		int b;
		for(int k=(14+8);k<data.length;k++){
			b=(int)(data[k]&0xff);
			if(b<=0xf){
				out.append("0x0"+Integer.toHexString(b));
			}else{
				out.append("0x"+Integer.toHexString(b));
			}				
			out.append(",");
			totalSize++;
			if(totalSize%32==0)out.append("\r\n");
		}       
        
		out.setCharAt(out.length()-1,' '); //remove traling comma	
		out.append("};\r\n");			
		
		FileUtils.writeStringToFile(new File(path+outFile), out.toString());
		System.out.println("Outputting file: "+path+outFile);
		System.out.println("Size:"+totalSize+" bytes");
		
	}
	
	static long tickDiff=0;	
	static boolean first=true;
	private static void addEvent(int currentEvent,Track track, MidiEvent event,double tempo,double speedFactor){

		if(first && event.getTick()!=0)tickDiff=event.getTick();
		Double scaled=new Double(event.getTick()-tickDiff);

		double factor=speedFactor/(60000000/tempo);
		scaled=scaled * factor; 
		long l=scaled.longValue();
		event.setTick(l);	
		track.add(event);
		first=false;
	}
	
	
	
	private static void doReverseConvert() throws Exception{
		File file=new File(path+"sounds_extract.inc");
		List lines=FileUtils.readLines(file);
		
		
		
		
		for(int i=0;i<9;i++){
			
			int count=0;
			byte out[]=new byte[256];
			for(int j=0;j<16;j++){
				int index=(i*16)+j;
				String strLine=(String)lines.get(index);
				strLine=strLine.substring(strLine.indexOf(".byte")+5);
				String bytes[]=strLine.split(",");
		
				byte b;
				String s;
			
				for(int k=0;k<16;k++){
					s=bytes[k].trim().substring(2, 4);
					if(s.equals("10")){
						b=0;
					}
					int temp=Integer.parseInt(s,16);
					b=(byte) ((byte)temp&0xff);
					out[count]=b;
					count++;
				}
			}
			
			FileUtils.writeByteArrayToFile(new File(path+"\\raw\\sample-"+i+".raw"), out);
			System.out.println("Outputting file: "+path+"\\raw\\sample-"+i+".raw");
		}
		
	}
	
	private static void doFrequencyTable() throws Exception{
		double cpuFreq=28636360;
		double mixingPeriod=(1/cpuFreq)*1820;
		double waveCyclePeriod=mixingPeriod*256;
		File outFile=new File(path+"steptable.inc");	
		String notes[]={"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
		
		StringBuffer str=new StringBuffer();
		str.append("// Step Table\n");
		str.append("// ---------------------------\r\n");
		str.append("// CPU=28.63636Mhz,");
		str.append("// Mixing Freqency="+(1/mixingPeriod)+" Hz\r\n\r\n");
		
		//f = 212k/12 × 440 Hz = 2k × 440 Hz
		//freq = 440 * 2^((n-69)/12)
		
		double a4Freq=440;
		int currNote=0;
		for(int note=0;note<127;note++){
			double dblNote=new Double(note).doubleValue();
			double noteFreq=a4Freq*Math.pow(2, (dblNote-69)/12);
						
			double step=(noteFreq/(1/waveCyclePeriod));
			Double dblConv=new Double(Math.round(step*256));				
			int stepConv=dblConv.intValue();
			
			str.append(".word 0x"+pad(Integer.toHexString(stepConv)));
			str.append(" // Note: "+note+" ("+notes[(currNote%12)]+"-"+((currNote/12)+1)+"), MIDI note: "+note+", Freq:"+noteFreq+", Step:"+step+"\r\n");
			currNote++;

		}			

		
		   FileUtils.writeStringToFile(outFile, str.toString());		   
		   System.out.println("Processing file "+outFile.getName()+"...Done!");
		   
	}
	
	private static String pad(String str){
		StringBuffer b=new StringBuffer();
		for(int i=0;i<(4-str.length());i++){
			b.append("0");
		}
		b.append(str);
		return b.toString();
	}
	
	private static void createPatterns() throws Exception{
		byte[] pic=new byte[210*200];
		
		int pos=0;
		byte col=0;
		for(int y=0;y<200;y++){
			for(int i=0;i<=9;i++)pic[pos++]=0; //filler
			
			
			for(int x=0;x<192;x++){
				pic[pos++]=col;	
				
			}
			
			for(int i=0;i<=9;i++)pic[pos++]=0; //filler
		}
		
		
		
		FileUtils.writeByteArrayToFile(new File(path+"gamut2.inc"),pic);
		
		System.out.println("Ouputting patterns...Done!");
		
	}
	
	private static void doPictureToTilesAsm(String inFilename,String outFilename,int horizontalTiles,int verticalTiles,int tileWidth,int tileHeight,int mapSize,int mapStart) throws Exception {
		File picFile=new File(path+"kernel\\data\\"+outFilename+".pic.inc");
		File inFile=new File(inFilename);
		
		System.out.println("Processing file "+inFile.getName()+"...");
		
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
		StringBuffer str=new StringBuffer("//picture: "+ inFile.getName() +"\r\n");
		str.append("//Horizontal Tiles="+horizontalTiles+"\r\n");
		str.append("//Vertical Tiles="+verticalTiles+"\r\n");
		str.append("//Tile Width="+tileWidth+"\r\n");
		str.append("//Tile Height="+tileHeight+"\r\n");
		
		List uniqueTiles=new LinkedList();
		
		int[] imageTiles=new int[horizontalTiles*verticalTiles];
			            
		int count=0;
		
		for(int v=0;v<verticalTiles;v++){
			for(int h=0;h<horizontalTiles;h++){
				
				byte[] tile=new byte[tileWidth*tileHeight];
				int tileIndex=0;
				for(int th=0;th<tileHeight;th++){
					for(int tw=0;tw<tileWidth;tw++){
						int index=(v*horizontalTiles*tileWidth*tileHeight)+(h*tileWidth)+(th*horizontalTiles*tileWidth)+ tw;
						tile[tileIndex++]=in[index];												
					}				
				}
				
				int refIndex=-1;				
				//check if tile exist
				for(int i=uniqueTiles.size()-1;i>=0;i--){
					byte[] b=(byte[])uniqueTiles.get(i);
					if(Arrays.equals(b,tile)){
						refIndex=i;
						break;
					}
				}
				
				if(refIndex==-1){
					uniqueTiles.add(tile); 
					imageTiles[count]=uniqueTiles.size()-1;
					
					boolean allZero=true;
					for(int i=0;i<tile.length;i++){
						if(tile[i]!=0)allZero=false;
					}
					if(allZero)
						System.out.println("Blank Tile index="+(uniqueTiles.size()-1));
					
				}else{
					imageTiles[count]=refIndex;
				}
				
				count++;
			}						
		}
		
		
		//build tile files from unique tiles
		for(Iterator it=uniqueTiles.iterator();it.hasNext();){
			byte[] tile=(byte[])it.next();
			
			for(int y=0;y<tileHeight;y++){
				str.append(".byte ");
				
				for(int x=0;x<tileWidth;x++){
					if(x>0)str.append(",");
					
					int o=tile[(y*tileHeight)+x] & 0xff;
					if(o<=0xf){
						str.append("0x0"+Integer.toHexString(o));
					}else{
						str.append("0x"+Integer.toHexString(o));
					}
				}
				
				str.append("\r\n");	
			}
			str.append("\r\n");	
			/*
			for(int index=0;index<(tileWidth*tileHeight);index++){
				;
				int o=tile[index] & 0xff;
				if(o<=0xf){
					str.append("0x0"+Integer.toHexString(o));
				}else{
					str.append("0x"+Integer.toHexString(o));
				}
				str.append("\r\n");
			}
			*/
		}

		
		FileUtils.writeStringToFile(picFile, str.toString());	
		System.out.println("Writing file "+picFile.getName()+" done!");

		
	    System.out.println("Unique tiles: "+uniqueTiles.size()+", size="+uniqueTiles.size()*tileWidth*tileHeight+" bytes");
	    System.out.println("Map size= "+(imageTiles.length*2)+" bytes");
	    int totalSize=(uniqueTiles.size()*tileWidth*tileHeight)+(imageTiles.length*2);
	    System.out.println("Total Size="+totalSize+" bytes");
	}

	//convert a monochrome tilseset
	private static void doMonoPictureToTilesAsm(String inFilename,String outFilename,int horizontalTiles,int verticalTiles,int tileWidth,int tileHeight,int mapSize,int mapStart) throws Exception {
		File picFile=new File(path+"kernel\\data\\"+outFilename+".pic.inc");
		File inFile=new File(inFilename);
		
		System.out.println("Processing file "+inFile.getName()+"...");
		
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
		StringBuffer str=new StringBuffer("//picture: "+ inFile.getName() +"\r\n");
		str.append("//Horizontal Tiles="+horizontalTiles+"\r\n");
		str.append("//Vertical Tiles="+verticalTiles+"\r\n");
		str.append("//Tile Width="+tileWidth+"\r\n");
		str.append("//Tile Height="+tileHeight+"\r\n");
		
		List uniqueTiles=new LinkedList();
		
		int[] imageTiles=new int[horizontalTiles*verticalTiles];
			            
		int count=0;
		
		for(int v=0;v<verticalTiles;v++){
			for(int h=0;h<horizontalTiles;h++){
				
				byte[] tile=new byte[tileWidth*tileHeight];
				int tileIndex=0;
				for(int th=0;th<tileHeight;th++){
					for(int tw=0;tw<tileWidth;tw++){
						int index=(v*horizontalTiles*tileWidth*tileHeight)+(h*tileWidth)+(th*horizontalTiles*tileWidth)+ tw;
						tile[tileIndex++]=in[index];												
					}				
				}
				
				int refIndex=-1;				
				//check if tile exist
				for(int i=uniqueTiles.size()-1;i>=0;i--){
					byte[] b=(byte[])uniqueTiles.get(i);
					if(Arrays.equals(b,tile)){
						refIndex=i;
						break;
					}
				}
				
				if(refIndex==-1){
					uniqueTiles.add(tile); 
					imageTiles[count]=uniqueTiles.size()-1;
					
					boolean allZero=true;
					for(int i=0;i<tile.length;i++){
						if(tile[i]!=0)allZero=false;
					}
					if(allZero)
						System.out.println("Blank Tile index="+(uniqueTiles.size()-1));
					
				}else{
					imageTiles[count]=refIndex;
				}
				
				count++;
			}						
		}
		
		//build tile files from unique tiles
		int tileNo=0;
		for(Iterator it=uniqueTiles.iterator();it.hasNext();){
			byte[] tile=(byte[])it.next();
			str.append(".byte ");
			//pack eight bytes to 1
			for(int y=0;y<tileHeight;y++){				
				int packed=0;
				int bit=7;
				for(int x=0;x<tileWidth;x++){
					if(tile[(y*tileHeight)+x]==0){
						packed |= (0 << bit);
					}else{
						packed |= (1 << bit);
					}
					bit--;
				}
				
				if(y>0)str.append(",");	
				if(packed<=0xf){
					str.append("0x0"+Integer.toHexString(packed));
				}else{
					str.append("0x"+Integer.toHexString(packed));
				}
				
			}
			str.append(" ;tile: "+ tileNo +"\r\n");			
			tileNo++;
		}
		
		FileUtils.writeStringToFile(picFile, str.toString());	
		System.out.println("Writing file "+picFile.getName()+" done!");

		
	    System.out.println("Unique tiles: "+uniqueTiles.size()+", size="+uniqueTiles.size()*6*8+" bytes");
	    int totalSize=(uniqueTiles.size()*6*8)+(imageTiles.length*2);
	    System.out.println("Total Size="+totalSize/8+" bytes");
	}
	
	private static void doPictureToTiles(String inFilename,String outFilename,int horizontalTiles,int verticalTiles,int tileWidth,int tileHeight,TileMap[] maps,int mapPointersSize) throws Exception {
		File picFile=new File(path+"data\\"+outFilename+".pic.inc");
		File mapFile=new File(path+"data\\"+outFilename+".map.inc");
		File inFile=new File(inFilename);
		
		System.out.println("Processing file "+inFile.getName()+"...");
		
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
		StringBuffer str=new StringBuffer("//picture: "+ inFile.getName() +"\r\n");
		str.append("//Horizontal Tiles="+horizontalTiles+"\r\n");
		str.append("//Vertical Tiles="+verticalTiles+"\r\n");
		str.append("//Tile Width="+tileWidth+"\r\n");
		str.append("//Tile Height="+tileHeight+"\r\n");
		
		List uniqueTiles=new LinkedList();
		
		int[] imageTiles=new int[horizontalTiles*verticalTiles];
			            
		int count=0;
		
		for(int v=0;v<verticalTiles;v++){
			for(int h=0;h<horizontalTiles;h++){
				
				byte[] tile=new byte[tileWidth*tileHeight];
				int tileIndex=0;
				for(int th=0;th<tileHeight;th++){
					for(int tw=0;tw<tileWidth;tw++){
						int index=(v*horizontalTiles*tileWidth*tileHeight)+(h*tileWidth)+(th*horizontalTiles*tileWidth)+ tw;
						tile[tileIndex++]=in[index];												
					}				
				}
				
				int refIndex=-1;				
				//check if tile exist
				for(int i=uniqueTiles.size()-1;i>=0;i--){
					byte[] b=(byte[])uniqueTiles.get(i);
					if(Arrays.equals(b,tile)){
						refIndex=i;
						break;
					}
				}
				
				if(refIndex==-1){
					uniqueTiles.add(tile); 
					imageTiles[count]=uniqueTiles.size()-1;
					
					boolean allZero=true;
					for(int i=0;i<tile.length;i++){
						if(tile[i]!=0)allZero=false;
					}
					if(allZero)
						System.out.println("Blank Tile index="+(uniqueTiles.size()-1));
					
				}else{
					imageTiles[count]=refIndex;
				}
				
				count++;
			}						
		}
		
		//build tile files from unique tiles
		str.append("#define "+outFilename.toUpperCase()+"_TILESET_SIZE "+uniqueTiles.size()+"\r\n\r\n");
		str.append("const unsigned char "+outFilename+"_tileset[] PROGMEM ={\r\n");
		int c=0,t=0;
		for(Iterator it=uniqueTiles.iterator();it.hasNext();){
			byte[] tile=(byte[])it.next();
			
			
			
			for(int index=0;index<(tileWidth*tileHeight);index++){
				if(c>0)str.append(",");
				int o=tile[index] & 0xff;
				if(o<=0xf){
					str.append(" 0x0"+Integer.toHexString(o));
				}else{
					str.append(" 0x"+Integer.toHexString(o));
				}
				c++;
			}
			str.append("\t\t //tile:"+t+" \r\n");
			t++;
			
		}
		str.append("};\r\n");

		
		//build maps 
		if(maps!=null && maps.length>0){			
			StringBuffer mapStr=new StringBuffer();
			int index=0;
			
			for(int m=0;m<maps.length;m++){
				TileMap map=maps[m];
				
				mapStr.append("const unsigned char map_"+map.getName()+"[] PROGMEM ={\r\n");
				mapStr.append(Integer.toString(map.getWidth())+",");
				mapStr.append(Integer.toString(map.getHeight()));
				c=0;
				for(int y=map.getY();y<(map.getY()+map.getHeight());y++){
					for(int x=map.getX();x<map.getX()+map.getWidth();x++){
						
						if(c%20==0){
							mapStr.append("\r\n"); //wrap lines
						}
						mapStr.append(",");
						index=(y*horizontalTiles)+x;
						
						if(imageTiles[index]>0xff && mapPointersSize==8)
							throw new RuntimeException("Tile index "+index+" greater than 255.");
						
						if(mapPointersSize==8){
							mapStr.append(toHex8(imageTiles[index]));
						}else{
							mapStr.append(toHex(imageTiles[index]));
						}
						
						c++;
						
					}
				}
				mapStr.append("};\r\n");
			
			}
			
			FileUtils.writeStringToFile(mapFile, mapStr.toString());
			System.out.println("Writing file "+mapFile.getName()+" done!");
		}
		
		
		FileUtils.writeStringToFile(picFile, str.toString());	
		System.out.println("Writing file "+picFile.getName()+" done!");

		
	    System.out.println("Unique tiles: "+uniqueTiles.size()+", size="+uniqueTiles.size()*6*8+" bytes");
	    int totalSize=0;
	    //if(mapSize>0){	
//	    	System.out.println("Map size= "+(imageTiles.length*2)+" bytes");
	//    	totalSize=(uniqueTiles.size()*6*8)+(imageTiles.length*2);
	  //  }else{
	    //	totalSize=(uniqueTiles.size()*6*8);
	    //}
	    System.out.println("Total Size="+totalSize+" bytes");
	}
	
	
	
	


	
	//converts a raw bitmap to an include file
	private static void convertRawToIncludeFile(String file,String type) throws Exception {
		File inFile=new File(path+"data\\"+file+".raw");
		File outFile=new File(path+"data\\"+file+".inc");		
				
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
		StringBuffer str=new StringBuffer("//"+type+": "+ inFile.getName() +"\r\n");
		str.append("#define sizeof_"+file+" "+in.length+"\r\n\r\n");
		str.append("const char "+file+"[] PROGMEM ={\r\n");
		
		for(int i=0;i<in.length;i++){
			int o=in[i] & 0xff ;
			if(o<=0xf){
				str.append("0x0"+Integer.toHexString(o));
			}else{
				str.append("0x"+Integer.toHexString(o));
			}
			
			if(i<(in.length-1))str.append(",");
		
			if((i%16)==0)str.append("\r\n");
					
		}
		
		
		str.append("};\r\n");
	   FileUtils.writeStringToFile(outFile, str.toString());
	     
	   System.out.println("Processing file "+inFile.getName()+"...Done!");
		
	}
	
	/**
	 * Generate the UZEBOX palette. File format is Photoshop palette (.act) 
	 * Palette is in 3:3:2 RGB format. 3 bits allocated for reg, 3 bits 
	 * allocated for green and 2 bits allocated for blue
	 * 
	 * @throws Exception
	 */
	private static void doPalette() throws Exception {

		File outFile=new File(path+"uzebox.act");
		
		int col=0;
		int palIndex=0;
		
		byte[] pal=new byte[(256*3)];//+4]; 
		
	
		//pal[768+0]=0;
		//pal[768+1]=(byte)0xff;
		//pal[768+2]=(byte)0xff;
		//pal[768+3]=(byte)0xff;
		
		for(int i=0;i<256;i++){

			pal[palIndex+0]= (byte)((((col>>0) & 7) * 255 /7))  ; //red
			
			
			pal[palIndex+1]= (byte)((((col>>3) & 7) * 255 /7))  ; //green
	
			
			pal[palIndex+2]= (byte)((((col>>6) & 3) * 255 /3))  ;  //blue

			
			palIndex+=3;
			col++;
		}
		
	    
		
		FileUtils.writeByteArrayToFile(outFile,pal);
		
		System.out.println("Ouputting palette...Done!");
	}

	private static void doTiles() throws Exception {
		
		String file="mario-block";
		File inFile=new File(path+"gfx\\"+file+".raw");
		//File outBinFile=new File(path+"gfx\\mario2.bin");
		File outFile=new File(path+file+".inc");
		
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
			
		//check if file is sprite file
		String fileType=new String(in,0,6,"UTF-8");
		if(!fileType.equals("SPRITE")) throw new Exception("Header 'SPRITE' not foud in file: "+inFile.getName());
		
		int tilesX=in[TILE_X_POS];
		int tilesY=in[TILE_Y_POS];
		
		byte[] out=new byte[tilesX*tilesY*16];
		int outPos=0;
		
		//int totalTiles=(pix.length-DATA_START_POS)/8;
		
		for(int y=0;y<tilesY;y++){
			for(int x=0;x<tilesX;x++){
				for(int row=0;row<8;row++){
					int inPos=DATA_START_POS+(x*8)+(tilesX*8*row)+(y*tilesX*8*8);
					
					//big endian
					int msb=( (in[inPos]&1)  <<0 | (in[inPos]&2)  <<4-1 |
							  (in[inPos+1]&1)<<1 | (in[inPos+1]&2)<<5-1 |
							  (in[inPos+2]&1)<<2 | (in[inPos+2]&2)<<6-1 |
							  (in[inPos+3]&1)<<3 | (in[inPos+3]&2)<<7-1 );
				
					inPos+=4;
					
					int lsb=( (in[inPos]&1)  <<0 | (in[inPos]&2)  <<4-1 |
							  (in[inPos+1]&1)<<1 | (in[inPos+1]&2)<<5-1 |
							  (in[inPos+2]&1)<<2 | (in[inPos+2]&2)<<6-1 |
							  (in[inPos+3]&1)<<3 | (in[inPos+3]&2)<<7-1 );
					
					
					byte b=(byte)(msb&0xff);
					byte b2=(byte)(lsb&0xff);
					
					out[outPos+0]=b;
					out[outPos+1]=b2;
								
					outPos+=2;				
				}
			}
		}
		
		//FileUtils.writeByteArrayToFile(outBinFile, out);
		//convert to assembly include file

		StringBuffer str=new StringBuffer("//"+ inFile.getName() +"tiles ("+tilesX+"x"+tilesY+")\r\n");

		for(int i=0;i<out.length/16;i++){
			str.append(".byte ");
			for(int j=0;j<16;j++){
				if(j>0)str.append(",");
				int o=out[(i*16)+j] & 0xff ;
				if(o<=0xf){
					str.append("0x0"+Integer.toHexString(o));
				}else{
					str.append("0x"+Integer.toHexString(o));
				}
			}
			str.append("\r\n");
		}
		
	   FileUtils.writeStringToFile(outFile, str.toString());
	     
		System.out.println("Processing file "+inFile.getName()+"...Done!");
		
	}

	private static void convertWave(String inFile, String destFile,String desc) throws Exception{

		byte[] in=FileUtils.readFileToByteArray(new File(inFile));		
		File outFile=new File(destFile);	
		
		StringBuffer str=new StringBuffer("//Wave: "+desc+"\r\n//File="+inFile);
		
		for(int i=0;i<in.length/16;i++){
			str.append(".byte ");
			for(int j=0;j<16;j++){
				if(j>0)str.append(",");
				int o=(in[(i*16)+j] & 0xff) ;
				if(o<=0xf){
					str.append("0x0"+Integer.toHexString(o));
				}else{
					str.append("0x"+Integer.toHexString(o));
				}
			}
			str.append("\r\n");
		}
		
	   FileUtils.writeStringToFile(outFile, str.toString());
	     
	System.out.println("Processing file "+inFile+"...Done!");
		
	}

	private static void doSrollingTable(){
		File outFile=new File(path+"kernel\\data\\scrolltable.inc");
	
		StringBuffer str=new StringBuffer("//X Scrolling table\r\n");
		str.append("const unsigned char scrolltable[][] PROGMEM ={\r\n");
		int scroll=0,fine=0; 
		for(int i=0;i<(32*6);i++){
			if(i>0)str.append(",\r\n");
			str.append("{"+scroll+","+fine+"}");
			fine++;
			if(fine==6){
				fine=0;
				scroll++;
			}
			
		}
		str.append("};\r\n");
		try {
			FileUtils.writeStringToFile(outFile, str.toString());
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Processing file "+outFile+"...Done!");
	}
	
	
}

