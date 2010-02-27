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
package com.belogic.uzebox.tools.converters.gfx;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
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
	


	
	protected static final int TILE_X_POS=6;
	protected static final int TILE_Y_POS=7;
	protected static final int DATA_START_POS=8;
	//private static String path="C:\\Documents and Settings\\Admin\\My Documents\\projet\\atmel\\uzebox2\\";
	private static String path="c:\\work\\uzebox\\sources\\rev4-beta\\";
		
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
		
		TileMap[] uzeboxlogoMaps8x8={
				new TileMap("uzeboxlogo",0,0,5,4),
				new TileMap("uzeboxlogo2",0,4,5,3),
			};
		
		
		TileMap[] composerMaps={
					new TileMap("title",1,1,24,3)
					};
		/*
		TileMap[] arkanoidMaps={
				//new TileMap("main",0,1,22,26),
				//new TileMap("frame",0,27,22,3),
				new TileMap("bg",0,1,5,5),
				new TileMap("logo",0,30,22,5),
				new TileMap("bar",21,30,1,5)
		};
		*/
		TileMap[] colorMaps={
				new TileMap("colorbars",0,0,40,28)
				};
		
		TileMap[] gameMaps={
				new TileMap("main",0,0,22,26)
				};
		
		TileMap[] smbMaps={
				new TileMap("hud",0,0,32,4),
				new TileMap("main",0,4,32,28)
				//new TileMap("opt0",0,0,2,2),
				//new TileMap("opt1",2,0,2,2),
				//new TileMap("opt2",4,0,2,2)
				};
	
		TileMap[] smbMapsLong={
				new TileMap("hud",0,0,28,4),
				new TileMap("main",0,4,640,22)
				};
		
		TileMap[] smbSprMaps={
				new TileMap("rwalk1",0,0,2,3),
				new TileMap("rwalk2",2,0,2,3),
				new TileMap("rskid",4,0,2,3),
				new TileMap("rjump1",6,0,2,3),
				new TileMap("rjump2",8,0,2,3),
				
				new TileMap("lwalk1",10,0,2,3),
				new TileMap("lwalk2",12,0,2,3),
				new TileMap("lskid",14,0,2,3),
				new TileMap("ljump1",16,0,2,3),
				new TileMap("ljump2",18,0,2,3),
				
				new TileMap("lgoomba1",20,0,2,2),
				new TileMap("lgoomba2",22,0,2,2),
				new TileMap("rgoomba1",24,0,2,2),
				new TileMap("rgoomba2",26,0,2,2),

		};
		

		TileMap[] arkanoidMaps={				
				new TileMap("wall",0,3,1,27),
				new TileMap("ceiling",0,2,28,1),
				
				new TileMap("bg1",1,3,8,4),
				new TileMap("bg2",1,7,8,4),
				new TileMap("bg3",1,11,8,4),
				new TileMap("bg4",1,15,8,4),
				
				new TileMap("smallVaus",23,0,2,1),
				
				new TileMap("brick0",1,0,2,1),
				new TileMap("brick1",3,0,2,1),
				new TileMap("brick2",5,0,2,1),
				new TileMap("brick3",7,0,2,1),
				new TileMap("brick4",9,0,2,1),
				new TileMap("brick5",11,0,2,1),
				new TileMap("brick6",13,0,2,1),
				new TileMap("brick7",15,0,2,1),
				new TileMap("brick8",17,0,2,1),
				new TileMap("brick9",19,0,2,1),
				new TileMap("brick10",21,0,2,1),
				
				new TileMap("brick0Anim1",0,1,2,1),
				new TileMap("brick0Anim2",2,1,2,1),
				new TileMap("brick0Anim3",4,1,2,1),
				new TileMap("brick0Anim4",6,1,2,1),
				new TileMap("brick0Anim5",8,1,2,1),
				
				new TileMap("brick1Anim1",10,1,2,1),
				new TileMap("brick1Anim2",12,1,2,1),
				new TileMap("brick1Anim3",14,1,2,1),
				new TileMap("brick1Anim4",16,1,2,1),
				new TileMap("brick1Anim5",18,1,2,1),	
				
				new TileMap("warpAnim1",10,3,1,5),
				new TileMap("warpAnim2",12,3,1,5),
				new TileMap("warpAnim3",14,3,1,5),
				new TileMap("warpAnim4",16,3,1,5),
				new TileMap("warpAnim5",18,3,1,5),
				
				
		};

		TileMap[] arkanoidTitleMaps={
				new TileMap("title",0,0,25,6)
		};
		
		TileMap[] arkanoidSpritesMaps={
				new TileMap("vaus1",0,1,5,2),
				new TileMap("vaus2",5,1,5,2),
				new TileMap("vaus3",10,1,5,2),
				new TileMap("vaus4",15,1,5,2),	
				
				new TileMap("vausLong1",0,7,7,2),
				new TileMap("vausLong2",7,7,7,2),
				new TileMap("vausLong3",14,7,7,2),
				new TileMap("vausLong4",21,7,7,2),
				
				new TileMap("vausdie1",0,3,5,2),
				new TileMap("vausdie2",5,3,5,2),
				new TileMap("vausdie3",10,3,5,2),
				new TileMap("vausdie4",15,3,5,2),
				new TileMap("vausdie5",21,3,5,2),
				new TileMap("vausdie6",27,3,5,2),
				
				new TileMap("vauswarp1",0,5,5,2),
				new TileMap("vauswarp2",5,5,5,2),
				new TileMap("vauswarp3",10,5,5,2),
				new TileMap("vauswarp4",15,5,5,2),
				new TileMap("vauswarp5",20,5,5,2),
				new TileMap("vauswarp6",25,5,5,2),
				
				new TileMap("vausLeftHalf",0,1,2,2),
				new TileMap("vausRightHalf",2,1,3,2),
				new TileMap("vausMiddle",2,1,1,2),
				
				new TileMap("vausTransformLaser1",0,9,5,2),
				new TileMap("vausTransformLaser2",5,9,5,2),
				new TileMap("vausTransformLaser3",10,9,5,2),
				new TileMap("vausTransformLaser4",15,9,5,2),
				new TileMap("vausTransformLaser5",20,9,5,2),
				
				new TileMap("vausLaser1",0,11,5,2),
				new TileMap("vausLaser2",5,11,5,2),
				new TileMap("vausLaser3",10,11,5,2),
				new TileMap("vausLaser4",15,11,5,2),
				
				new TileMap("round",15,0,5,1),
				new TileMap("ready",20,0,5,1),
				
				new TileMap("s_powerup1",0,13,2,1),
				new TileMap("s_powerup2",2,13,2,1),
				new TileMap("s_powerup3",4,13,2,1),
				new TileMap("s_powerup4",6,13,2,1),
				
				new TileMap("e_powerup1",8,13,2,1),
				new TileMap("e_powerup2",10,13,2,1),
				new TileMap("e_powerup3",12,13,2,1),
				new TileMap("e_powerup4",14,13,2,1),

				new TileMap("d_powerup1",16,13,2,1),
				new TileMap("d_powerup2",18,13,2,1),
				new TileMap("d_powerup3",20,13,2,1),
				new TileMap("d_powerup4",22,13,2,1),

				new TileMap("l_powerup1",24,13,2,1),
				new TileMap("l_powerup2",26,13,2,1),
				new TileMap("l_powerup3",28,13,2,1),
				new TileMap("l_powerup4",30,13,2,1),

				new TileMap("c_powerup1",0,14,2,1),
				new TileMap("c_powerup2",2,14,2,1),
				new TileMap("c_powerup3",4,14,2,1),
				new TileMap("c_powerup4",6,14,2,1),

				new TileMap("b_powerup1",8,14,2,1),
				new TileMap("b_powerup2",10,14,2,1),
				new TileMap("b_powerup3",12,14,2,1),
				new TileMap("b_powerup4",14,14,2,1),

				new TileMap("p_powerup1",16,14,2,1),
				new TileMap("p_powerup2",18,14,2,1),
				new TileMap("p_powerup3",20,14,2,1),
				new TileMap("p_powerup4",22,14,2,1)


				
		};
		
		TileMap[] yiearMaps={
				new TileMap("main",0,0,30,28)
		};
		
		/*
		TileMap[] arkanoidIntroMaps={
				new TileMap("introMap",0,6,28,28),
				new TileMap("trustersAnim1",1,0,12,6),
				new TileMap("trustersAnim2",3,25,12,6)
		};
		*/
		TileMap[] arkanoidIntroMaps={
				new TileMap("introMap",0,0,14,10)				
		};
		
		TileMap[] cursorMaps={
				new TileMap("cursor",0,0,3,3),
				new TileMap("hammerUp",3,0,3,3),
				new TileMap("hammerDown",6,0,3,3)
		};
		
		TileMap[] mouseMaps={
				new TileMap("mouse",0,0,20,9)
		};
		
		TileMap[] moleMaps={
				new TileMap("main",1,9,27,22),
				new TileMap("mole1",1,0,6,4),
				new TileMap("mole2",7,0,6,4),
				new TileMap("mole3",13,0,6,4),
				new TileMap("mole4",19,0,6,4),
				new TileMap("mole5",1,4,6,4),
				new TileMap("mole6",7,4,6,4),
				new TileMap("mole7",13,4,6,4),
				new TileMap("hole",19,4,6,4),
				new TileMap("startBtnUp",25,0,5,2),
				new TileMap("startBtnDown",25,2,5,2),
				new TileMap("ready",25,4,7,2),
				new TileMap("gameOver",34,4,9,2),
				new TileMap("whack",25,6,7,2)
		};
		TileMap[] moleTitleMaps={
				new TileMap("title",1,0,22,3)
		};
		
		TileMap[] uzeampMaps={
				new TileMap("main",0,0,30,27),
				new TileMap("digitSep",3,4,1,3),
				new TileMap("digit0",4,4,2,3),
				new TileMap("digit1",6,4,2,3),
				new TileMap("digit2",8,4,2,3),
				new TileMap("digit3",10,4,2,3),
				new TileMap("digit4",12,4,2,3),
				new TileMap("digit5",14,4,2,3),
				new TileMap("digit6",16,4,2,3),
				new TileMap("digit7",18,4,2,3),
				new TileMap("digit8",20,4,2,3),
				new TileMap("digit9",22,4,2,3),
				new TileMap("digitBlank",24,4,2,3),
				
				new TileMap("btnPrevNormal",0,28,3,2),
				new TileMap("btnPrevPushed",15,28,3,2),
				
				new TileMap("btnPlayNormal",3,28,3,2),
				new TileMap("btnPlayPushed",18,28,3,2),
				
				new TileMap("btnPauseNormal",6,28,3,2),
				new TileMap("btnPausePushed",21,28,3,2),
				
				new TileMap("btnStopNormal",9,28,3,2),
				new TileMap("btnStopPushed",24,28,3,2),
				
				new TileMap("btnNextNormal",12,28,3,2),
				new TileMap("btnNextPushed",27,28,3,2)
				
		};
		
		TileMap[] uzeampSpritesMaps={
				new TileMap("hCursor",0,0,2,1),
				new TileMap("vCursor",2,0,1,1),
				new TileMap("mouse",4,0,1,2)
		};
		
		/*
		 * The following commented calls are examples to build tile sets and convert
		 * MIDI files for the uzebox.
		 * 
		 */	
		
		convertRawToIncludeFile(path+"demos\\BitmapDemo\\data","sprite",2,16,30);
		
		//doPictureToTiles(path+"tetris\\tetris-tiles.raw","tetrisTiles",40,46,6,8,tetrisMaps,16);
		//doPictureToTiles(path+"gfx\\fonts_full.raw","fontsFull",91,1,6,8,null,8);
		//doPictureToTiles(path+"composer\\composer_6x8.raw","composerTiles",28,6,6,8,composerMaps,16);		
		
		//doPictureToTiles(path+"gfx\\sprite_test.raw","spr",17,1,6,8,null,16);
		//doPictureToTiles(path+"gfx\\arkanoid-tiles.raw","arkanoid",22,35,6,8,arkanoidMaps,8);	
//		convertSprites(path+"gfx\\arkanoid-sprites.raw","arkanoid_sprites",22,2,6,8,null,8);
		//doPictureToTiles(path+"gfx\\colorbars.raw","colorbars",40,28,6,8,colorMaps,16);
		
	//	convertRawToIncludeFile("uzeboxbeat","PCM sample");
			
		//convertRawToIncludeFile("spark","PCM sample");
		//convertRawToIncludeFile("bomb","PCM sample");
	//	convertRawToIncludeFile("hat_tite","PCM sample");
	
		//doPictureToTiles(path+"gfx\\game.raw","game",22,26,6,8,gameMaps,8);
		
		
		//doPictureToTiles(path+"demos\\MouseTest\\data\\fonts_8x8.raw", path+"demos\\MouseTest\\data\\fonts_8x8","font_tileset",16,4,8,8,null,8);
		//doPictureToTiles(path+"demos\\MouseTest\\data\\uzeboxlogo_8x8.raw", path+"demos\\MouseTest\\data\\uzeboxlogo_8x8","logo_tileset",5,7,8,8,uzeboxlogoMaps8x8,8);

		//doPictureToTiles(path+"demos\\Uzeamp\\data\\font8x8_blue_small.raw", path+"demos\\Uzeamp\\data\\font8x8blue","font_tileset_blue",16,4,8,8,null,8);
		//doPictureToTiles(path+"demos\\Uzeamp\\data\\font8x8.raw", path+"demos\\Uzeamp\\data\\font8x8","font_tileset",32,2,8,8,null,8);
		//doPictureToTiles(path+"demos\\Uzeamp\\data\\main.raw",path+"demos\\Uzeamp\\data\\main","main_tileset",30,30,8,8,uzeampMaps,8);
		//doPictureToTiles(path+"demos\\Uzeamp\\data\\sprites.raw",path+"demos\\Uzeamp\\data\\sprites","sprites_tileset",6,2,8,8,uzeampSpritesMaps,8);
		
		//convertRawToIncludeFile(path+"demos\\VideoDemo\\data\\frame64b","frame");
		 //convertRawToMovieFrame(path+"demos\\VideoDemo\\data\\fram170");
		
		 //convertRawToMovieFrame("F:\\temp\\badluck_export\\raw\\Sequence","F:\\temp\\badluck_export\\raw\\Sequence.raw","f:\\badluck.uzm",9615); 
		// convertRawToMovieFrame("F:\\temp\\output\\matrix","F:\\temp\\export\\Sequence01.raw","f:\\matrix.uzm",4316); 
		 
		//doPictureToTiles(path+"demos\\Arkanoid\\data\\map.raw",path+"demos\\Arkanoid\\data\\map","arkanoid_tileset",28,30,8,8,arkanoidMaps,8);
		//doPictureToTiles(path+"demos\\Arkanoid\\data\\title.raw",path+"demos\\Arkanoid\\data\\title","arkanoidTile_tileset",25,6,8,8,arkanoidTitleMaps,8);
		//doPictureToTiles(path+"demos\\Arkanoid\\data\\sprites.raw",path+"demos\\Arkanoid\\data\\sprites","arkanoidSprites_tileset",34,15,8,8,arkanoidSpritesMaps,8);
		//doPictureToTiles(path+"demos\\Arkanoid\\data\\intro2.raw",path+"demos\\Arkanoid\\data\\intro","intro_tileset",14,10,8,8,arkanoidIntroMaps,8);
		//doPictureToTiles(path+"demos\\Arkanoid\\data\\font.raw",path+"demos\\Arkanoid\\data\\font","font_tileset",16,3,8,8,null,8);
		
		//doPictureToTiles(path+"demos\\MouseTest\\data\\cursor.raw",path+"demos\\MouseTest\\data\\cursor","cursor_tileset",16,16,8,8,cursorMaps,8);
		//doPictureToTiles(path+"demos\\MouseTest\\data\\tiles.raw",path+"demos\\MouseTest\\data\\tiles","tiles_tileset",20,9,8,8,mouseMaps,8);
	
		//doPictureToTiles(path+"demos\\MouseTest\\data\\mole.raw",path+"demos\\MouseTest\\data\\mole","mole_tileset",43,31,8,8,moleMaps,8);
		//doPictureToTiles(path+"demos\\MouseTest\\data\\title.raw",path+"demos\\MouseTest\\data\\title","title_tileset",23,3,8,8,moleTitleMaps,8);
		//convertSong("\\demos\\MouseTest\\data\\Whacksong\\Whacksong.mid","\\demos\\MouseTest\\data\\Whacksong.inc","song_Whacksong",34);
		
		//doPictureToTiles(path+"demos\\Arkanoid\\data\\yiear.raw",path+"demos\\Arkanoid\\data\\yiear","yiear_tileset",30,28,8,8,yiearMaps,8);
		
		//doPictureToTiles(path+"demos\\SuperMarioDemo\\data\\marioworld_map_long.raw",path+"demos\\SuperMarioDemo\\data\\smb","smb_tileset",640,26,8,8,smbMapsLong,8);
		//doPictureToTiles(path+"demos\\SuperMarioDemo\\data\\marioworld_map.raw",path+"demos\\SuperMarioDemo\\data\\smb","smb_tileset",32,32,8,8,smbMaps,8);
		//doPictureToTiles(path+"demos\\NewEngine\\data\\smw_sprites.raw",path+"demos\\NewEngine\\data\\mario_sprites","mario_sprites_tileset",36,3,8,8,smbSprMaps,8);
		
		//doPictureToTiles(path+"gfx\\arkanoid-sprites.raw","arkanoid_sprites",22,2,6,8,null,8);
		//convertSprites(path+"gfx\\arkanoid-sprites.raw","arkanoid_sprites",22,2,6,8,null,8);
		
//		doMonoPictureToTilesAsm(path+"gfx\\fonts_8x8.raw","fonts8x8",16,4,8,8,0,0);
	//	doPictureToTilesAsm(path+"gfx\\fonts_8x8_reduced.raw","fonts8x8reduced",44,1,8,8,0,0);
		 
		//convertSong("demos\\Megatris\\data\\Korobeiniki-3tracks.mid","demos\\Megatris\\data\\Korobeiniki-3tracks.inc","song_korobeiniki",29);
		//convertSong("demos\\Megatris\\data\\testrisnt__0.mid","demos\\Megatris\\data\\testrisnt.inc","song_testrisnt",33);
		//convertSong("demos\\Megatris\\data\\ending2.mid","demos\\Megatris\\data\\ending.inc","song_ending",30);
		
		//convertSong("demos\\MusicDemo\\data\\tetrisnt_fast.mid","demos\\MusicDemo\\data\\testrisnt_fast.inc","song_testrisnt_fast",30,false);
		//convertSong("demos\\MusicDemo\\data\\ghost.mid","demos\\MusicDemo\\data\\ghost.inc","song_ghost",29,true);
		//convertSong("demos\\MusicDemo\\data\\dr_mario_03_uzebox_final.mid","demos\\MusicDemo\\data\\drmario_main.inc","song_drmario_main",30,true);
		//convertSong("demos\\MusicDemo\\data\\NSMB.mid","demos\\MusicDemo\\data\\nsmb.inc","song_nsmb",30,true);		
		
		//convertSong("music\\pumpkin_f1b.mid","data\\pumpkin.inc","song_pumpkin",160);
		
		//convertSong("\\demos\\Arkanoid\\data\\arkanoid1_converted.mid","\\demos\\Arkanoid\\data\\arkanoid1_converted.inc","song_arkanoid1",29);
		//convertSong("\\demos\\Arkanoid\\data\\arkanoid2_converted.mid","\\demos\\Arkanoid\\data\\arkanoid2_converted.inc","song_arkanoid2",30);
		
		//convertSong("\\demos\\pacman-sound\\music\\newgame2.mid","\\demos\\pacman-sound\\music\\newgame.inc","song_newgame",32 ,true);
		//convertSong("\\demos\\pacman-sound\\music\\intermission.mid","\\demos\\pacman-sound\\music\\intermission.inc","song_intermission",30 ,true);
		
		//convertSong("\\demos\\MusicDemo\\data\\moonpatrol.mid","\\demos\\MusicDemo\\data\\moonpatrol.inc","song_moonpatrol",72);
		
		//convertSong("\\_music\\dr_mario_03_uzebox_final.mid","\\demos\\MusicDemo\\data\\drmario_main.inc","song_drmario_main",60);
		
		//convertSong("sound\\sparkfun_0.mid","include\\sparkfun.inc","sparkfun_song",60);
		
		//convertWave(path+"\\kernel\\waves\\sample-10.raw", path+"\\kernel\\waves\\sample-10.inc","3 octaves square wave");
		

		
	//	convertFont("d:\\temp\\fonts.raw","C:\\work\\uzebox\\sources\\rev4-beta\\demos\\Bootloader\\fonts.inc",63 );
		//convertFont("d:\\temp\\fonts_full.raw","C:\\work\\uzebox\\sources\\rev4-beta\\demos\\Bootloader\\fonts_full.inc",91 ); 
		
		//doSdTestFile();
		
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
	
	
	private static void doBitmap(){
		
	}
	
	private static void doSdTestFile() {
		File dataFile=new File("f:\\work\\uzebox\\sd\\test.bin");
		byte[] data=new byte[512*256];
		
		for(int i=0;i<256;i++){
			for(int j=0;j<512;j++){
				data[(i*512)+j]=(byte)(i&0xff);
			}			
		}
		
		try {
			FileUtils.writeByteArrayToFile(dataFile, data);
		} catch (IOException e) {
			e.printStackTrace();
		}
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
	
	private static void doPictureToTiles(String inFilename,String outFilename,String tilsetName, int horizontalTiles,int verticalTiles,int tileWidth,int tileHeight,TileMap[] maps,int mapPointersSize) throws Exception {
		File picFile=new File(outFilename+".pic.inc");
		File mapFile=new File(outFilename+".map.inc");
		File inFile=new File(inFilename);
		
		System.out.println("Processing file "+inFile.getName()+"...");
		
		//byte[] in=FileUtils.readFileToByteArray(inFile);
		byte[] in=getBytesFromFile(inFile);
		
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
		str.append("#define "+tilsetName.toUpperCase()+"_SIZE "+uniqueTiles.size()+"\r\n\r\n");
		str.append("const char "+tilsetName+"[] PROGMEM ={\r\n");
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
				mapStr.append("#define MAP_"+map.getName().toUpperCase()+"_WIDTH "+map.getWidth()+"\r\n");
				mapStr.append("#define MAP_"+map.getName().toUpperCase()+"_HEIGHT "+map.getHeight()+"\r\n");
				mapStr.append("const char map_"+map.getName()+"[] PROGMEM ={\r\n");
				
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
				mapStr.append("};\r\n\r\n");
			
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
	private static void convertRawToIncludeFile(String path,String name,int bbp,int width,int height) throws Exception {
		if(!path.endsWith(File.separator))path+=File.separator;
			
		File inFile=new File(path+name+".raw");
		File outFile=new File(path+name+".inc");		
				
		byte[] in=FileUtils.readFileToByteArray(inFile);
		
		StringBuffer str=new StringBuffer("//"+name+": "+ inFile.getName() +"\r\n");

		
		if(bbp==8){
			str.append("#define sizeof_"+name+" "+in.length+"\r\n\r\n");
			str.append("const char "+name+"[] PROGMEM ={\r\n");
			
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
		}else{
			str.append("#define sizeof_"+name+" "+in.length/4 +"\r\n");
			str.append("#define "+name+"_width "+width +"\r\n");
			str.append("#define "+name+"_height "+height +"\r\n\r\n");
			
			List palette=new LinkedList();
			for(int i=0;i<in.length;i++){
				if(!palette.contains(in[i])){
					palette.add(in[i]);
				}
			}
			if(palette.size()>4)throw new RuntimeException("Too many colors in source bitmap. Max 4, found:"+palette.size());
			str.append("const char "+name+"_palette[] PROGMEM ={");
			str.append(palette.get(0)+","+palette.get(1)+","+palette.get(2)+","+palette.get(3));
			str.append("};\r\n");
			
			
			str.append("const char "+name+"[] PROGMEM ={\r\n");
			for(int i=0;i<in.length;i+=4){
				byte pix1=(byte) palette.indexOf(in[i+0]);
				byte pix2=(byte) palette.indexOf(in[i+1]);
				byte pix3=(byte) palette.indexOf(in[i+2]);
				byte pix4=(byte) palette.indexOf(in[i+3]);
				str.append((pix1<<6)|(pix2<<4)|(pix3<<2)|(pix4<<0));
				
				if(i<(in.length-4))str.append(",");				
				if((i%16)==0)str.append("\r\n");
			}

		}
		
		
		str.append("};\r\n");
	   FileUtils.writeStringToFile(outFile, str.toString());
	     
	   System.out.println("Processing file "+inFile.getName()+"...Done!");
		
	}
	
	//converts a raw bitmap to an include file
	private static void convertRawToMovieFrame2(String file) throws Exception {
		
		File outFile=new File("f:\\matrix.uzm");	
		if(outFile.exists()){
			outFile.delete();
		}
			
		FileOutputStream fs=FileUtils.openOutputStream(outFile);
		
		
		byte[] out=new byte[512*38];
		byte[] in;
		
		for(int f=1;f<4317;f++){
			for(int t=0;t<2;t++){ //double field
				File inFile=new File("f:\\temp\\output\\matrix"+pad(f)+".raw");
				in=FileUtils.readFileToByteArray(inFile);
				
				
				int inPos=0;
				int outPos=0;
				for(int i=0;i<in.length;i++){
					out[outPos++]=in[i];
					inPos++;
					if(inPos==510){
						inPos=0;
						out[outPos++]=0;
						out[outPos++]=0;
					}
				}
				IOUtils.write(out, fs);
			}
			
			System.out.println("Processing frame: "+f);
		}
		
		fs.flush();
		fs.close();
		
	   //FileUtils.writeByteArrayToFile(outFile, out);
	   System.out.println("Processing file "+file+"...Done!");
		
	}
	
	private static String pad(int i){
		
		if(i<10){
			return "000"+i;
		}else if(i<100){
			return "00"+i;
		}else if(i<1000){
			return "0"+i;
		}else{
			return ""+i;
		}
		
		
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
	
	
	
	
	private static void convertRawToMovieFrame(String framesPath,String audioPath,String outPath,int frameCount) throws Exception {
		//String outFileName="f:\\badluck.uzm";
		String outFileName=outPath;
		
		File outFile=new File(outFileName);	
		if(outFile.exists()){
			outFile.delete();
		}
			
		FileOutputStream fs=FileUtils.openOutputStream(outFile);
		
//		byte[] audio=FileUtils.readFileToByteArray(new File("F:\\temp\\badluck_export\\raw\\Sequence.raw"));
		byte[] audio=FileUtils.readFileToByteArray(new File(audioPath));
		//byte[] audioChunk=new byte[512];
		     
		byte[] out=new byte[512*39];
		byte[] in;
		int audioPos=0;
		
		for(int f=1;f<frameCount;f++){
		
			for(int t=0;t<2;t++){ //double field

				//write audio chunk for field
				for(int c=0;c<262;c++){
					out[c]=audio[audioPos++];	
				}
				
				//File inFile=new File("F:\\temp\\badluck_export\\raw\\Sequence"+pad(f)+".raw");
				File inFile=new File(framesPath+pad(f)+".raw");
				in=FileUtils.readFileToByteArray(inFile);
				
				
				int inPos=0;
				int outPos=512;
				for(int i=0;i<in.length;i++){
					out[outPos++]=in[i];
					inPos++;
					if(inPos==510){
						inPos=0;
						out[outPos++]=0;
						out[outPos++]=0;
					}
				}
				IOUtils.write(out, fs);
			}
			
			System.out.println("Processing frame: "+f);
		}
		
		fs.flush();
		fs.close();
		
	   //FileUtils.writeByteArrayToFile(outFile, out);
	   System.out.println("Done processing file: "+outFileName+" !");
		
	}
	
	
	
	private static void convertFont(String inFile,String destFile, int widthInChar) throws Exception{

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
	
    public static byte[] getBytesFromFile(File file) throws IOException {
        InputStream is = new FileInputStream(file);
    
        // Get the size of the file
        long length = file.length();
    
        if (length > Integer.MAX_VALUE) {
            // File is too large
        }
    
        // Create the byte array to hold the data
        byte[] bytes = new byte[(int)length];
    
        // Read in the bytes
        int offset = 0;
        int numRead = 0;
        while (offset < bytes.length
               && (numRead=is.read(bytes, offset, bytes.length-offset)) >= 0) {
            offset += numRead;
        }
    
        // Ensure all the bytes have been read in
        if (offset < bytes.length) {
            throw new IOException("Could not completely read file "+file.getName());
        }
    
        // Close the input stream and return bytes
        is.close();
        return bytes;
    }
	
}