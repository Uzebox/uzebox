package org.uzebox.tools.converters.xm;

import java.io.File;
import java.io.IOException;
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Arrays;

import org.apache.commons.io.FileUtils;
import org.apache.log4j.Logger;

public class UzeboxModule {
	static Logger logger = Logger.getLogger(UzeboxModule.class);
	
	private int step=64;
	private int channels=5;
	private ArrayList<TrackPattern> patterns;
	private Order[] orders;
	private int restartPosition;
	private int tempo;
	
		
	public UzeboxModule(XmModule mod){
		
		patterns=new ArrayList<TrackPattern>();
		orders=new Order[mod.getSongLenght()];
		this.restartPosition=mod.getRestartPosition();
		this.tempo=mod.getTempo();
		
		
		//build output orders and unique track patterns
		for(int i=0;i<mod.getSongLenght();i++){
			Pattern inPat=mod.getPatternAtOrder(i);
			orders[i]=new Order(channels);
			
			for(int chan=0;chan<channels;chan++){
				int patIndex=patterns.indexOf(inPat.tracks[chan]);
				if(patIndex==-1){
					//if(inPat.tracks[chan].isBlank()){
					//	patIndex=0xff;	//empty pattern marker
					//}else{
						patterns.add(inPat.tracks[chan]);
						patIndex=patterns.size()-1;
					//}
				}
				orders[i].patternIndexes[chan]=patIndex;						
			}
			
			logger.debug("Order "+i+ ": "+ orders[i].toString()+"\r\n");
			
		}
		
		/*
		DecimalFormat fmt2 = new DecimalFormat("00");
		
		StringBuffer str=new StringBuffer("\r\n  ");
		for(int p=0;p<patterns.size();p++){	
			str.append("  Pattern:"+fmt2.format(p)+"  ");
		}
		str.append("\r\n  ");
		for(int p=0;p<patterns.size();p++){	
			str.append("+-------------");
		}
		str.append("\r\n");
		for(int row=0;row<64;row++){
			str.append(fmt2.format(row));
			for(int p=0;p<patterns.size();p++){							
				str.append(patterns.get(p).getRows()[row].toString());
			}	
			str.append("\r\n");
		}
		logger.debug(str.toString());
		*/
		
		
		logger.debug("Unique track patterns:"+patterns.size());
	}
	
	public void export(File outfile, String varName){
		DecimalFormat fmt = new DecimalFormat("00");
		
		//byte[] data=new byte[65536]; //max possible size on uzebox
		
		/* File format is:
		 * 
		 * <Header>
		 * <orders>
		 * <pattern offsets>
		 * <patterns table>
		 * 
		 */
		
		
		
		
		/*
		 * generate header
		 * 
		 * byte:header size
		 * byte:channels
		 * byte:track patterns count
		 * byte:step
		 * byte:tempo
		 * byte:song length
		 * byte:restartPosition
		 * 
		 */
		int headerSize=7;

		IncludeFileWriter out=new IncludeFileWriter(outfile);
		
		out.addByte(headerSize);
		out.addByte(channels);
		out.addByte(patterns.size());
		out.addByte(patterns.get(0).getLenght());
		out.addByte(tempo);
		out.addByte(orders.length);
		out.addByte(restartPosition);

		//ouput orders for track patterns (order*channels)
		for(int i=0;i<orders.length;i++){
			for(int j=0;j<channels;j++){
				out.addByte(orders[i].patternIndexes[j]);
			}
		}
		
		//output pattern offsets			
		int packedPatternSize=0;
		for(int i=0;i<patterns.size();i++){
			int size=this.packPattern(i).length;					
			out.addWord(packedPatternSize);
			packedPatternSize+=size;
			logger.debug("Packed Pattern size "+i+": "+size);
		}
		
		//output patterns
		for(int i=0;i<patterns.size();i++){
			out.addByteArray(this.packPattern(i));
		}
		
		int totalSize=(headerSize+(orders.length*channels)+(patterns.size()*2)+packedPatternSize);
		
		//Output include C file
		out.setFileComment("Uzebox module v1.0\r\nFile: "+outfile.getAbsolutePath()+"\r\nData size:"+totalSize+"\r\n");
		out.setVarName(varName);
		out.write();
			
				
		logger.info("Packed Pattern Data size:"+packedPatternSize);
		logger.info("Song size patterns+orders:"+totalSize);
		logger.info("File written:"+outfile.getAbsolutePath());
		logger.info("Done!");
		
	}
	/*
	private void writeByteArray(byte[] data,int ptr,byte[] ba){
		for(int i=0;i<ba.length;i++){
			data[ptr++]=ba[i];
		}
	}
	
	private void writeByte(byte[] data,int ptr,byte b){
		data[ptr++]=(byte)(b&0xff);
	}
	
	private void writeByte(byte[] data,int ptr,int i){
		data[ptr++]=(byte)(i&0xff);
	}
	
	//little indian
	private void writeWord(byte[] data,int ptr,int i){
		data[ptr++]=(byte)(i&0xff);
		data[ptr++]=(byte)((i>>8)&0xff);
	}
	*/
	
	private byte[] packPattern(int index){
		TrackPattern pat=patterns.get(index);
		byte[] data=new byte[5*pat.getLenght()]; //max size
		
		/*Pack with format:
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
		int pos=0;
		for(int i=0;i<pat.getLenght();i++){
			TrackPattern.Row row=pat.getRow(i);
			byte b1=0,b2=0,b3=0,b4=0,b5=0;
			boolean b2Set=false,b3Set=false,b4Set=false,b5Set=false;
			
			if(row.isNoteSet()){
				b1=(byte)((row.getNote()&0x7f)-1);
			}
			if(row.isInstrumentSet()||row.isVolumeSet()||row.isEffectTypeSet()){
				b1|=0x80;
			}
			
			if(row.isInstrumentSet()||row.isVolumeSet()||row.isEffectTypeSet()){
				b2=0;
				b2Set=true;
				if(row.isInstrumentSet()) b2|=0b00100000;
				if(row.isVolumeSet()) 	  b2|=0b01000000;
				if(row.isEffectTypeSet()) b2|=0b10000000;
								
				if(row.isInstrumentSet()){
					b2|=(byte)((row.getInstrument()-1)&0x1f); //XM instruments are "1" based, patches "0" based
									
					if(row.isVolumeSet()){
						b3=(byte)((row.getVolume()>>1)&0x1f);
						b3Set=true;
						if(row.isEffectTypeSet()){
							b4|=(byte)((row.getEffectType())&0x1f);
							b5|=(byte)((row.getEffectParam())&0x1f);
							b4Set=true;
							b5Set=true;
						}
					}else if(row.isEffectTypeSet()){
						b3|=(byte)((row.getEffectType())&0x1f);
						b4|=(byte)((row.getEffectParam())&0x1f);
						b3Set=true;
						b4Set=true;
					}
					
				}else if(row.isVolumeSet()){
					b2|=(byte)((row.getVolume()>>1)&0x1f);
					if(row.isEffectTypeSet()){
						b3|=(byte)((row.getEffectType())&0x1f);
						b4|=(byte)((row.getEffectParam())&0x1f);
						b3Set=true;
						b4Set=true;
					}					
				}else if(row.isEffectTypeSet()){
					b2|=(byte)((row.getEffectType())&0x1f);					
				}
			}

			data[pos++]=b1;
			if(b2Set) data[pos++]=b2;
			if(b3Set) data[pos++]=b3;
			if(b4Set) data[pos++]=b4;
			if(b5Set) data[pos++]=b5;
			
		}
		
		byte[] packed=new byte[pos];
		packed=Arrays.copyOf(data, pos);
				
		return packed;
	}
	
}





