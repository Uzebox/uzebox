package org.uzebox.tools.converters.xm;

import java.io.File;
import java.text.DecimalFormat;
import java.util.ArrayList;

import org.apache.commons.io.FileUtils;
import org.apache.log4j.Logger;


public class XmConvert {
	static Logger logger = Logger.getLogger(XmConvert.class);
	
	private final static String DEFAULT_VARIABLE_NAME="xmsong";
	
	private File inputFile=null;
	private File outputFile=null;
	private String variableName=DEFAULT_VARIABLE_NAME;
	
	
	public static void main (String [] args) throws Exception{
		System.out.println("Uzebox (tm) XM module converter 1.0");
		System.out.println("(c)2014 Alec Bourque. This tool is released under the GNU GPL V3.");
		System.out.println("");
		
		String basePath="C:/work/uzebox/trunk/demos/MusicDemo_Tempest2000/data/";
		
		XmConvert conv=new XmConvert();
		conv.setInputFile(new File(basePath+"mindseye_3.xm"));
		conv.setOutputFile(new File(basePath+"mindseye.inc"));
		conv.convertSong();
		
	}
	

	public File getInputFile() {
		return inputFile;
	}


	public void setInputFile(File inputFile) {
		this.inputFile = inputFile;
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
	
	
	public void convertSong(){
		
		XmModule mod=new XmModule();
		mod.load(this.inputFile);

		XmModule mod2=mod.resizePatterns(64,16);
		
		UzeboxModule uze=new UzeboxModule(mod2);
		uze.export(outputFile,"mindseye");
		
		
	}

	

	
	
}

