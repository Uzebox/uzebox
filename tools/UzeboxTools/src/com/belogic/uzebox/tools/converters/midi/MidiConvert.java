package com.belogic.uzebox.tools.converters.midi;

import java.io.File;
import java.util.HashMap;
import java.util.Map;

import javax.sound.midi.MetaMessage;
import javax.sound.midi.MidiEvent;
import javax.sound.midi.MidiFileFormat;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.Sequence;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Track;

import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.GnuParser;
import org.apache.commons.cli.HelpFormatter;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.io.FileUtils;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;


public class MidiConvert {
	static Logger logger = Logger.getLogger(MidiConvert.class);

	private final static double DEFAULT_SPEED_FACTOR=30;
	private final static String DEFAULT_VARIABLE_NAME="midisong";
	
	private static final int CONTROLER_TREMOLO=92;
	private static final int CONTROLER_TREMOLO_RATE=100;
	private static final int CONTROLER_VOL=7;
	private static final int CONTROLER_EXPRESSION=11;
	
	private File inputFile=null;
	private File outputFile=null;
	private String variableName=DEFAULT_VARIABLE_NAME;
	private double speedFactor=DEFAULT_SPEED_FACTOR;
	private int loopStartTick=-1,loopEndTick=-1;
	private boolean includeNoteOff=false;
	
	public static void main (String [] args) throws Exception{
		System.out.println("Uzebox (tm) MIDI converter 1.0");
		System.out.println("(c)2009 Alec Bourque. This tool is released under the GNU GPL V3.");
		System.out.println("");
		
		
		try{
			Options options = new Options();
			options.addOption("v", true, "variable name used in the include file. Defaults to '"+DEFAULT_VARIABLE_NAME+"'");
			options.addOption("s", true, "Force a loop start (specified in tick). Any existing loop start in the input will be discarded.");
			options.addOption("e", true, "Force a loop end (specified in tick). Any existing loop end in the input will be discarded.");
			options.addOption("f", true, "Speed correction factor (double). Defaults to "+DEFAULT_SPEED_FACTOR);
			options.addOption("o", false, "Include note off messages. Note off can be explicit note-off or note-on with zero volume.");
			options.addOption("h", false, "Prints this screen.");
			options.addOption("d", false, "Prints debug info.");
			
			if(args.length==0){
				printHelp(options);
				System.exit(0);
			}
			
			
			CommandLineParser parser = new GnuParser();
			CommandLine cmd = parser.parse( options, args);

			if(cmd.hasOption("d")) {
				logger.setLevel(Level.DEBUG);
			}
			
			if(cmd.hasOption("h")) {
				printHelp(options);
				System.exit(0);			
			}
			
			MidiConvert converter=new MidiConvert();		
			converter.setVariableName(cmd.getOptionValue("v",DEFAULT_VARIABLE_NAME));
			converter.setLoopStartTick(Integer.parseInt(cmd.getOptionValue("s", "-1")));
			converter.setLoopEndTick(Integer.parseInt(cmd.getOptionValue("e", "-1")));
			converter.setSpeedFactor(Double.parseDouble(cmd.getOptionValue("f",Double.toString(DEFAULT_SPEED_FACTOR))));
			if(cmd.hasOption("o"))converter.setIncludeNoteOff(true);
			

			
			//remaining args must be the input & output filenames
			if(cmd.getArgs().length!=2){
				System.out.print("Invalid command arguments. Only two non-option argument must be specified and it must be the input & output filenames.\r\n");
				printHelp(options);		
				System.exit(0);
			}else{
				converter.setInputFile(new File(cmd.getArgs()[0]));
				converter.setOutputFile(new File(cmd.getArgs()[1]));
			}
	
			if((converter.getLoopEndTick()==-1 && converter.getLoopStartTick()!=-1) ||
				(converter.getLoopEndTick()!=-1 && converter.getLoopStartTick()==-1)){
				
				System.out.print("Invalid command arguments. Loop end & loop start but both be specified.");
				System.exit(0);				
			}else if((converter.getLoopStartTick()>=converter.getLoopEndTick()) && converter.getLoopEndTick()!=-1 && converter.getLoopStartTick()!=-1){
				System.out.print("Invalid command arguments. Loop start must be smaller than loop end.");
				System.exit(0);
			}
			
			logger.debug(converter);
			converter.convertSong();
	
		}catch(ParseException exp ) {
		      // oops, something went wrong
			  System.err.println( "Parsing failed.  Reason: " + exp.getMessage() );
		  }
		  
	}
		
	private static void printHelp(Options options){
		HelpFormatter formatter = new HelpFormatter();
		formatter.printHelp("midiconv [options] inputfile outputfile", 
							"Converts a MIDI song in format 0 to a Uzebox MIDI stream outputted as a C include file.\r\n",options,
							"Ex: midiconv -s32 -vmy_song -ls200 -le22340 c:\\mysong.mid c:\\mysong.inc \r\n" );			
	}
	
	/**
	 * Converts a multi track MIDI song to a single track song and remove useless events. 
	 * 
	 * @param filename
	 * @param outFile
	 * @throws Exception
	 */
	private void convertSong() throws Exception{	
		
		//define the supported commands
		Map <Integer,Command>commands=new HashMap<Integer,Command>();
		commands.put(new Integer(0x90),new Command("Note On",true));
		commands.put(new Integer(0xb0),new Command("Controller",true));
		commands.put(new Integer(0xc0),new Command("Program Change",true));
		commands.put(new Integer(0xe0),new Command("Pitch Bend",false));

		//define meta events
		Map <Integer,MetaEvent>metaEvents=new HashMap <Integer,MetaEvent>();
		metaEvents.put(new Integer(0x2),new MetaEvent("Copyright",false));
		metaEvents.put(new Integer(0x3),new MetaEvent("Track Name",false));
		metaEvents.put(new Integer(0x6),new MetaEvent("Marker",true));
		metaEvents.put(new Integer(0x2f),new MetaEvent("End of Track",true));
		metaEvents.put(new Integer(0x51),new MetaEvent("Tempo",true));
		metaEvents.put(new Integer(0x58),new MetaEvent("Time Signature",false));
		metaEvents.put(new Integer(0x59),new MetaEvent("Key Signature",false));
		
		//create in/out files
		//File inFile=new File(path+filename);
		
		MidiFileFormat format=MidiSystem.getMidiFileFormat(inputFile);
		
		if(format.getType()!=0){
			throw new RuntimeException("Unsupported file format "+format.getType()+". Only MIDI file format 0 (all events in one track) is supported.");
		}
		
		Sequence inSequence=MidiSystem.getSequence(inputFile);		
		
		Sequence seq=new Sequence(format.getDivisionType(),format.getResolution(),1);
		Track outTrack=seq.getTracks()[0];
		
		long tempo=60000000/format.getResolution();

		Track track=inSequence.getTracks()[0];

		for(int e=0;e<track.size();e++){
			MidiEvent event=track.get(e);				
			
			if(event.getMessage() instanceof MetaMessage){
				MetaMessage m=(MetaMessage)event.getMessage();
				if(metaEvents.containsKey(m.getType()) && metaEvents.get(m.getType()).supported){
					if(m.getType()==6 &&  m.getLength()>4)
						throw new RuntimeException("META markers text size must by only one character: "+new String(m.getData()));
					
					if(m.getType()==0x51){ //tempo event
						byte[] t=m.getData();
						long tmp=(t[0]<<16)+(t[1]<<8)+t[2];
						tempo=tmp;
					}else if(!(m.getType()==6 && this.getLoopStartTick()!=-1) ){//ignore loop meta events if we specified some on command line
						addEvent(outTrack,event,tempo,speedFactor);
						logger.debug("META: tick="+event.getTick()+",type="+Integer.toHexString(m.getType())+":"+metaEvents.get(m.getType()).name);
					}
						
					
				}else{
					//String name="";
					//if(metaEvents.get(m.getType())!=null) name=metaEvents.get(m.getType()).name;
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
						if(m.getData1()==CONTROLER_VOL || m.getData1()==CONTROLER_EXPRESSION || m.getData1()==CONTROLER_TREMOLO || m.getData1()==CONTROLER_TREMOLO_RATE){
							addEvent(outTrack,event,tempo,speedFactor);
							logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"	+commands.get(m.getCommand()).name+"type="+m.getData1());								
						}else{
							//logger.debug("MIDI:tick="+tick+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"	+commands.get(m.getCommand()).name+"type="+m.getData1()+": UNSUPPORTED");
						}
					}else if(m.getCommand()==0x90 && m.getData2()==0){ //note off
						if(includeNoteOff){
							addEvent(outTrack,event,tempo,speedFactor);
							logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":Note Off");
						}else{
							logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":Note Off: IGNORED");
						}
					}else{
						addEvent(outTrack,event,tempo,speedFactor);
						logger.debug("MIDI:tick="+event.getTick()+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"	+commands.get(m.getCommand()).name);
					}
				}else{
					//String name="";
					//if(commands.get(m.getCommand())!=null) name=commands.get(m.getCommand()).name;
					//logger.warn("MIDI:tick="+tick+",channel="+m.getChannel()+",command=0x"+Integer.toHexString(m.getCommand())+":"+name+": UNSUPPORTED");
				}
			}else{
				//logger.warn("SYSEX: tick="+tick+": UNSUPPORTED");
			}

		}

		//add looping meta events if required
		if(this.getLoopStartTick()!=-1){
			MetaMessage metaS=new MetaMessage();
			metaS.setMessage(6, new byte[]{83}, 1); //loop start, "S" marker
			MidiEvent eventS=new MidiEvent(metaS,this.getLoopStartTick());
			addEvent(outTrack,eventS,tempo,speedFactor);
			
			MetaMessage metaE=new MetaMessage();
			metaE.setMessage(6, new byte[]{69}, 1); //loop end, "E" marker
			MidiEvent eventE=new MidiEvent(metaE,this.getLoopEndTick());
			addEvent(outTrack,eventE,tempo,speedFactor);			
		}
	
        File temp = File.createTempFile("~mid", ".tmp");        
        temp.deleteOnExit();
        MidiSystem.write(seq, 0, temp);
        
        byte data[]=FileUtils.readFileToByteArray(temp);
        
        //output track data skipping headers
		StringBuffer out=new StringBuffer();
		out.append("//*********************************//\r\n");
		out.append("// MIDI file: "+outputFile.getName()+"\r\n");
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
		
		FileUtils.writeStringToFile(outputFile, out.toString());
		System.out.println("Outputting file: "+outputFile.getAbsolutePath());
		System.out.println("Size:"+totalSize+" bytes");
		System.out.println("Done!");
	}
	
	static long tickDiff=0;	
	static boolean first=true;
	private static void addEvent(Track track, MidiEvent event,double tempo,double speedFactor){

		if(first && event.getTick()!=0)tickDiff=event.getTick();
		Double scaled=new Double(event.getTick()-tickDiff);

		double factor=speedFactor/(60000000/tempo);
		scaled=scaled * factor; 
		long l=scaled.longValue();
		event.setTick(l);	
		
		track.add(event);
		first=false;
	}


	public File getInputFile() {
		return inputFile;
	}


	public void setInputFile(File sourcefile) {
		this.inputFile = sourcefile;
	}


	public File getOutputFile() {
		return outputFile;
	}


	public void setOutputFile(File outputFile) {
		this.outputFile = outputFile;
	}


	public String getVariableName() {
		return variableName;
	}


	public void setVariableName(String variableName) {
		this.variableName = variableName;
	}


	public double getSpeedFactor() {
		return speedFactor;
	}


	public void setSpeedFactor(double speedFactor) {
		this.speedFactor = speedFactor;
	}


	public int getLoopStartTick() {
		return loopStartTick;
	}


	public void setLoopStartTick(int loopStartTick) {
		this.loopStartTick = loopStartTick;
	}


	public int getLoopEndTick() {
		return loopEndTick;
	}


	public void setLoopEndTick(int loopEndTick) {
		this.loopEndTick = loopEndTick;
	}


	public boolean isIncludeNoteOff() {
		return includeNoteOff;
	}


	public void setIncludeNoteOff(boolean includeNoteOff) {
		this.includeNoteOff = includeNoteOff;
	}

	public String toString(){
		return "Input file="+inputFile+", output file="+outputFile+", variable="+variableName+
		", speed factor="+speedFactor+", loop start="+loopStartTick+", loop end="+loopEndTick+", include note off="+includeNoteOff;
	} 
	
	
}
