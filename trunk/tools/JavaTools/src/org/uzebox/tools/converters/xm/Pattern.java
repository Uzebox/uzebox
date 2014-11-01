package org.uzebox.tools.converters.xm;

public class Pattern {
	
	public TrackPattern tracks[];
	
	public Pattern(int channelsCount,int patternRows){
		tracks=new TrackPattern[channelsCount];
		for(int chan=0;chan<channelsCount;chan++){
			tracks[chan]=new TrackPattern(patternRows);
		}		
	}
	

}
