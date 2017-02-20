package org.uzebox.tools.converters.gfx;

import java.io.File;

import org.apache.commons.io.FileUtils;
import org.uzebox.tools.converters.gfx.Main.Language;

public class AudioHelper {


	/**
	 * Generates the kernel's mixer frequency step table
	 * 
	 * @param outFile
	 * @throws Exception
	 */
	private static void doFrequencyTable(File outFile) throws Exception{
		double cpuFreq=28636360;
		double mixingPeriod=(1/cpuFreq)*1820;
		double waveCyclePeriod=mixingPeriod*256;

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
	

	/**
	 * Convert a raw signed audio file to an include file. 
	 * 
	 * @param inFile
	 * @param destFile
	 * @param desc
	 * @param var
	 * @param lang
	 * @throws Exception
	 */
	private static void convertWave(String inFile, String destFile,String desc, String var, Language lang ) throws Exception{

		byte[] in=FileUtils.readFileToByteArray(new File(inFile));		
		File outFile=new File(destFile);	
		StringBuffer str=new StringBuffer();
				
		if(lang.equals(Language.C)){
			str.append("//"+desc+"\r\n//File="+inFile+"\r\n");		
			str.append("#define sizeof_"+var+" ");
			str.append(in.length);
			str.append("\r\n");
			str.append("const char "+var+"[] PROGMEM ={\r\n");
			
			for(int i=0;i<in.length/16;i++){
				for(int j=0;j<16;j++){
					if(i>0 || j>0)str.append(",");
					int o=(in[(i*16)+j] & 0xff) ;
					if(o<=0xf){
						str.append("0x0"+Integer.toHexString(o));
					}else{
						str.append("0x"+Integer.toHexString(o));
					}
				}
				str.append("\r\n");
			}
			str.append("};\r\n");
			
		}else{
	        
			str.append("//Wave: "+desc+"\r\n//File="+inFile+"\r\n");	        
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
		}
		
	   FileUtils.writeStringToFile(outFile, str.toString());	     
	   System.out.println("Processing file "+inFile+"...Done!");
		
	}

	
	private static String pad(String str){
		StringBuffer b=new StringBuffer();
		for(int i=0;i<(4-str.length());i++){
			b.append("0");
		}
		b.append(str);
		return b.toString();
	}
    
}
