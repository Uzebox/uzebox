package org.uzebox.tools.converters.gfx;

import java.io.File;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.io.FileUtils;

public class GraphicsHelper {

	/**
	 * Converts a bitmap in RAW format to a C include file. Bitmap must be in 1 byte per index format (i.e palette).
	 * @param inputFile
	 * @param outputFile
	 * @param bpp bits per pixels. Valid values 8,4,1
	 * @param imageWidth width in pixels of source image
	 * @param imageHeight height in pixels of source image
	 */
	public static void convertRawImageToIncludeFile(File inputFile, File outputFile,String variableName,int bpp,int imageWidth,int imageHeight) throws Exception {
							
		byte[] in=FileUtils.readFileToByteArray(inputFile);
		int pixelsPerByte;
		
		StringBuffer str=new StringBuffer("//ConvertHelper.convertRawImageToIncludeFile()\r\n");
		str.append("//"+inputFile.getName() +"\r\n");

		//count distinct colors in source image
		List<Byte> palette=new LinkedList<Byte>();
		for(int i=0;i<in.length;i++){
			if(!palette.contains(in[i])){
				palette.add(in[i]);
			}
		}
		
		if(bpp==8){
			pixelsPerByte=1;
		}else if(bpp==4){
			if(palette.size()>4)throw new RuntimeException("Too many colors in source bitmap. Max 4, found:"+palette.size());
			pixelsPerByte=2;
		}else if(bpp==1){
			if(palette.size()>2)throw new RuntimeException("Too many colors in source bitmap. Max 2, found:"+palette.size());
			pixelsPerByte=8;
		}else{
			throw new Exception("Unsupported bpp value: "+bpp);
		}
		
		if((imageWidth*imageHeight)!=in.length) throw new RuntimeException("The specified image dimension (imageWidth*imageHeight) does not queal the input file's size");
		
		
		str.append("//Bits per pixel: "+bpp +"\r\n");
		str.append("#define "+variableName+"_size "+in.length / pixelsPerByte+"\r\n");
		str.append("#define "+variableName+"_width "+imageWidth +"\r\n");
		str.append("#define "+variableName+"_height "+imageHeight +"\r\n");
		str.append("const char "+variableName+"[] PROGMEM ={\r\n");
		
		if(bpp==8){
			
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
		}else if(bpp==4){
			

			
			str.append("const char "+variableName+"_palette[] PROGMEM ={");
			str.append(palette.get(0)+","+palette.get(1)+","+palette.get(2)+","+palette.get(3));
			str.append("};\r\n");
			
			
			str.append("const char "+variableName+"[] PROGMEM ={\r\n");
			for(int i=0;i<in.length;i+=4){
				byte pix1=(byte) palette.indexOf(in[i+0]);
				byte pix2=(byte) palette.indexOf(in[i+1]);
				byte pix3=(byte) palette.indexOf(in[i+2]);
				byte pix4=(byte) palette.indexOf(in[i+3]);
				str.append((pix1<<6)|(pix2<<4)|(pix3<<2)|(pix4<<0));
				
				if(i<(in.length-4))str.append(",");				
				if((i%16)==0)str.append("\r\n");
			}

		}else if(bpp==1){

	    	int b,i=0;
	    	
	    	for(int y=0;y<imageHeight;y++){
	    		for(int x=0;x<imageWidth;x+=8){
					if(x>0 || y>0)str.append(",");
					
	    			b=0;
	    			b|=(in[(y*imageWidth)+x+0])<<7;
	    			b|=(in[(y*imageWidth)+x+1])<<6;
	    			b|=(in[(y*imageWidth)+x+2])<<5;
	    			b|=(in[(y*imageWidth)+x+3])<<4;
	    			b|=(in[(y*imageWidth)+x+4])<<3;
	    			b|=(in[(y*imageWidth)+x+5])<<2;
	    			b|=(in[(y*imageWidth)+x+6])<<1;
	    			b|=(in[(y*imageWidth)+x+7]);

					b=(b&0xff);
					if(b<=0xf){
						str.append("0x0"+Integer.toHexString(b));
					}else{
						str.append("0x"+Integer.toHexString(b));
					}		
					i++;
					if((i%32)==0)str.append("\r\n");
					
	    		}
	    	}	
		}
		
		str.append("};\r\n");
		
		FileUtils.writeStringToFile(outputFile, str.toString());
	    System.out.println("Processing file "+inputFile.getName()+" done.");	    
	    System.out.println("Output data size: "+ (in.length / pixelsPerByte)+" bytes");
		
	}
	

	
	/**
	 * Generate a Photoshop palette file (.act) that models the UZebox fixed palette.
	 * The ACT file is made of 256 entries of 3 bytes for R,G and B values.  
	 *  
	 * Uzebox uses a fixed palette in 3:3:2 RGB format. 3 bits allocated for reg, 3 bits 
	 * allocated for green and 2 bits allocated for blue. The formulas
	 * models Clay Gowgill weighted DAC design, which is the official
	 * Uzebox DAC currently used by all implementations. 
	 * 
	 * @throws Exception
	 */
	public static void generatePaletteFile(File outFile) throws Exception {
		
		int col=0;
		int palIndex=0;
		
		byte[] pal=new byte[(256*3)]; 
		
		for(int i=0;i<256;i++){
			
			pal[palIndex+0]= (byte)((((col>>0) & 7) * 255 /7))  ; //red			
			pal[palIndex+1]= (byte)((((col>>3) & 7) * 255 /7))  ; //green				
			pal[palIndex+2]= (byte)((((col>>5) & 6) * 255 /7))  ;  //blue
			
			palIndex+=3;
			col++;
		}

		FileUtils.writeByteArrayToFile(outFile,pal);
		
		System.out.println("Ouputting palette file: "+outFile.getAbsolutePath()+"\r\nDone!");
	}

	/**
	 * Generates a special packed font for the bootloader. 
	 * 
	 * @param inFile
	 * @param destFile
	 * @param widthInChar
	 * @throws Exception
	 */
	public static void generateBootloaderFont(String inFile,String destFile, int widthInChar) throws Exception{

		byte[] in=FileUtils.readFileToByteArray(new File(inFile));		
		File outFile=new File(destFile);	
		
		StringBuffer str=new StringBuffer(";5x8 packed monochrome font for the bootloader\r\n");
		//str.append("#define FONT_SIZE "+widthInChar+"\r\n\r\n");
		
		//int l=(in.length/48)-1;
		for(int i=0;i<widthInChar;i++){
			
			str.append(".byte ");
			
			String pack="";
			
			for(int k=0;k<8;k++){
					
				byte b=0;
				int shift=7;
				for(int j=0;j<5;j++){
					if(in[(i*6)+(k*widthInChar*6)+j]!=0){
						b|=(1<<shift);
					}
					shift--;	
				}
		
				pack+=toBin(b);
			}

			System.out.println(pack);
			
			str.append("0b"+pack.substring(0,8)+",");
			str.append("0b"+pack.substring(8,16)+",");
			str.append("0b"+pack.substring(16,24)+",");
			str.append("0b"+pack.substring(24,32)+",");
			str.append("0b"+pack.substring(32,40));

			
			str.append(" ; #"+i+" = " +Character.toString((char)(i+32)));
			
			str.append("\r\n");
		}
		str.append(".align 2 ;insure remaining code is aligned on a word");
		
	   FileUtils.writeStringToFile(outFile, str.toString());
	     
	   System.out.println("Processing file "+inFile+"...Done!");
		
	}
	
	private static String toBin(byte b){
		int i=(b&0xff);
		String s=Integer.toBinaryString(i);
		int l=8-s.length();
		for(int j=0;j<l;j++)s="0"+s;
		//System.out.println(s);
		s=s.substring(0, 5);
		System.out.println(s);
		return s;
	}
}
