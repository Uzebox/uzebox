package org.uzebox.tools.converters.xm;

//groups many track patterns in a single regular MOD patterns 
public class Order {
	private int channels;
	int[] patternIndexes;
	private byte transpose=0;
	
	public Order(int channels){
		this.channels=channels;
		patternIndexes=new int[channels];
	}
	public int getChannels() {
		return channels;
	}
	public void setChannels(int channels) {
		this.channels = channels;
	}
	public int[] getPatternIndexes() {
		return patternIndexes;
	}
	public void setPatternIndexes(int[] patternIndexes) {
		this.patternIndexes = patternIndexes;
	}
	public byte getTranspose() {
		return transpose;
	}
	public void setTranspose(byte transpose) {
		this.transpose = transpose;
	}
	
	public String toString(){
		StringBuffer str=new StringBuffer();
		str.append("PatternSet[channels="+channels+", orders={");
		for(int i=0;i<patternIndexes.length;i++){
			if(i>0) str.append(",");
			str.append(patternIndexes[i]);
		}
		str.append("}]");
		return str.toString();
	}
	
}
