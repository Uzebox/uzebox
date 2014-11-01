package org.uzebox.tools.converters.xm;

import java.io.File;
import java.io.IOException;

import org.apache.commons.io.FileUtils;

public class IncludeFileWriter {

	private File file;
	private String varName;
	private String fileComment;
	byte[] data=new byte[65536]; //max possible size on uzebox
	int ptr=0;
	
	public IncludeFileWriter(File file){
		this.file=file;
	}
	
	public void addByteArray(byte[] ba){
		for(int i=0;i<ba.length;i++){
			data[ptr++]=ba[i];
		}
	}
	
	public void addByte(byte b){
		data[ptr++]=(byte)(b&0xff);
	}
	
	public void addByte(int b){
		data[ptr++]=(byte)(b&0xff);
	}
		
	//little indian
	public void addWord(int i){
		data[ptr++]=(byte)(i&0xff);
		data[ptr++]=(byte)((i>>8)&0xff);
	}	
	
	public void write(){
		//output to file
		StringBuffer buf=new StringBuffer();
		buf.append("/*\r\n");
		buf.append(fileComment);
		buf.append("*/\r\n");
		buf.append("const char "+varName+"[] PROGMEM ={\r\n");
		
			
		int b,cnt=0;
		for(int i=0;i<ptr;i++){
			b=(int)(data[i]&0xff);
			if(b<=0xf){
				buf.append("0x0"+Integer.toHexString(b));
			}else{
				buf.append("0x"+Integer.toHexString(b));
			}				
			buf.append(",");
			cnt++;
			if(cnt%32==0)buf.append("\r\n");
		}       
        
		buf.setCharAt(buf.length()-1,' '); //remove traling comma	
		buf.append("\r\n};\r\n");			
		
		try {
			FileUtils.writeStringToFile(file, buf.toString());
		} catch (IOException e) {
			throw new RuntimeException(e);
		}
		
		
	}

	public String getVarName() {
		return varName;
	}

	public void setVarName(String varName) {
		this.varName = varName;
	}

	public String getFileComment() {
		return fileComment;
	}

	public void setFileComment(String fileComment) {
		this.fileComment = fileComment;
	}
	
	
}
