package org.uzebox.tools.converters.xm;

import java.text.DecimalFormat;

public class TrackPattern {
	private int lenght;
	private Row rows[];
	
	public TrackPattern(int lenght){
		if(lenght<=0) throw new RuntimeException("Invalid trackPattern lenght. Must be > 0.");
		this.lenght=lenght;
		rows=new Row[lenght];
		for(int i=0;i<lenght;i++){
			rows[i]=new Row();
		}
		
	}

	public int getLenght() {
		return lenght;
	}

	public Row[] getRows() {
		return rows;
	}

	public Row getRow(int index){
		return rows[index];
	}
	
	public class Row{
		/*
		 * Offset Length Type Ref Example
				? 1 byte Note 01  Note = 1..96, 1 = C-0, 97 = key off 
				+1 1 byte Instrument 01 (0..128)
				+2 1 byte Volume column byte 00 (0..64, 255)
				+3 1 byte Effect type 05 (0..26)
				+4 1 byte Effect parameter 1F (0..255)
		 */
		int note;
		int instrument;
		int volume;
		int effectType;
		int effectParam;
		int flags=0;	//defines if bytes are set
		
		public int getFlags(){
			return flags;
		}
		
		public int getNote() {
			return note;
		}
		public void setNote(int note) {
			this.note = note;
			flags|=0x1;
		}
		public boolean isNoteSet(){
			return (flags&0x1)!=0;
		}
				
		public int getInstrument() {
			return instrument;
		}
		public void setInstrument(int instrument) {
			this.instrument = instrument;
			flags|=0x2;
		}
		public boolean isInstrumentSet(){
			return (flags&0x2)!=0;
		} 
		
		public int getVolume() {
			return volume;
		}
		public void setVolume(int volume) {
			this.volume = volume;
			flags|=0x4;
		}
		public boolean isVolumeSet(){
			return (flags&0x4)!=0;
		}
		
		public int getEffectType() {
			return effectType;
		}
		public void setEffectType(int effectType) {
			this.effectType = effectType;
			flags|=0x8;
		}
		public boolean isEffectTypeSet(){
			return (flags&0x8)!=0;
		}
		
		public int getEffectParam() {
			return effectParam;
		}
		public void setEffectParam(int effectParam) {
			this.effectParam = effectParam;
			flags|=0x10;
		}
		public boolean isEffectParamSet(){
			return (flags&0x10)!=0;
		}
		
		public String toString(){
			StringBuffer str=new StringBuffer("|");
			String notes[]={"C-","C#","D-","D#","E-","F-","F#","G-","G#","A-","A#","B-"} ;
			DecimalFormat fmt1 = new DecimalFormat("0");
			DecimalFormat fmt2 = new DecimalFormat("00");
			
			if(this.isNoteSet()){
				str.append(notes[(note-1)%12]);
				str.append(fmt1.format((note-1)/12));
				str.append(" ");
			}else{
				str.append("... ");
			}
			
			if(this.isInstrumentSet()){
				str.append(fmt2.format(instrument));
				str.append(" ");
			}else{
				str.append(".. ");
			}
			
			if(this.isVolumeSet()){
				str.append(fmt2.format(volume));
				str.append(" ");
			}else{
				str.append(".. ");
			}
			
			if(this.isEffectTypeSet()){
				str.append("X");
			}else{
				str.append(".");
			}
			
			if(this.isEffectParamSet()){
				str.append("YY");
			}else{
				str.append("..");
			}
			
			//str.append("|");
			
			return str.toString();
		}
				
				
	}
	
	public boolean isBlank(){
		for(int row=0;row<lenght;row++){
			if(!(rows[row].getFlags()==0)) return false;					
		}		
		return true;
	}
	
	public boolean equals(Object obj){
		 if ( this == obj ) return true;

		//use instanceof instead of getClass here for two reasons
		//1. if need be, it can match any supertype, and not just one class;
		//2. it renders an explict check for "that == null" redundant, since
		//it does the check for null already - "null instanceof [type]" always
		//returns false. (See Effective Java by Joshua Bloch.)
		if ( !(obj instanceof TrackPattern) ) return false;
		
		TrackPattern pat=(TrackPattern)obj;
		
		if(lenght!=pat.getLenght()) return false;
		
		//compare both patterns row by row	    
		for(int row=0;row<lenght;row++){
			if( !(rows[row].getFlags()==pat.getRows()[row].getFlags()) ||
				!(rows[row].getNote()==pat.getRows()[row].getNote()) ||
				!(rows[row].getInstrument()==pat.getRows()[row].getInstrument()) ||
				!(rows[row].getVolume()==pat.getRows()[row].getVolume())
				)return false;
		}
				
		return true;    
	}
	
	
}
