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
package org.uzebox.tools.converters.gfx;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Arrays;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.io.FileUtils;
import org.apache.commons.io.IOUtils;
import org.apache.log4j.Logger;



/**
 * @author Admin
 *
 */
public class Main {
	static Logger logger = Logger.getLogger(Main.class);
	
	public enum Language {C, ASM};

	private static String path="c:\\work\\uzebox\\trunk\\";

	
	public static void main (String [] args) throws Exception{

		File f=new File("c:\\temp\\ApplicationLog.log");
		f.delete();
			

	//	GraphicsHelper.convertRawImageToIncludeFile(
	//			new File("c://work//uzebox//git//demos//Mode13Demo//data//akuma2.raw"), 
	//			new File("c://work//uzebox//git//demos//Mode13Demo//data//akuma.inc"),
	//			"akuma",1,360,224);
		
		GraphicsHelper.generatePaletteFile(new File("c://work//uzebox//git//uzebox-new.act"));
		
	}
	
	



    
}