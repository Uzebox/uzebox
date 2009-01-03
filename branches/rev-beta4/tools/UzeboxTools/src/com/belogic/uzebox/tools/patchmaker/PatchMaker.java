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

/**
 * This program is a quick and dirty application to help making sound patches
 * for the uzebox game console.
 * 
 * When starting the program put in the text box the exact same format
 * as the patch file definition but without the first byte (patch type)  
 * 
 * i.e:
 * 
 * 0,PC_WAVE,1,
 * 1,PC_ENV_VOL,200,
 * 1,PC_NOTE_HOLD,0,
 * 1,PC_ENV_SPEED,-20, 
 * 0,PATCH_END
 * 
 * And press play...
 */

package com.belogic.uzebox.tools.patchmaker;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Graphics2D;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;

import javax.sound.midi.InvalidMidiDataException;
import javax.sound.midi.MidiDevice;
import javax.sound.midi.MidiSystem;
import javax.sound.midi.MidiUnavailableException;
import javax.sound.midi.Receiver;
import javax.sound.midi.ShortMessage;
import javax.sound.midi.Synthesizer;
import javax.sound.midi.SysexMessage;
import javax.swing.*;


public class PatchMaker extends JPanel implements ActionListener {
    protected JTextArea textField;
    protected JButton btnSend;
    protected JButton btnPlay;
    protected JTextArea textArea;
    protected JScrollPane areaScrollPane;
    ButtonGroup group; 
    
    protected MidiDevice midiDevice;
    protected Receiver receiver;
    
    protected String lastSend="";
    
	public static void main(String[] args) {
        //Schedule a job for the event-dispatching thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }
    private static void createAndShowGUI() {
        //Create and set up the window.
    	
		try {
			UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
		} catch (Exception e) {
			e.printStackTrace();
		}
		
        JFrame frame = new JFrame("Uzebox Patch Maker");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.add(new PatchMaker());

        frame.setExtendedState(Frame.MAXIMIZED_BOTH );
        frame.setVisible(true);

    }
    
	public PatchMaker(){
		super(new GridBagLayout());
		GridBagConstraints gridConst = new GridBagConstraints();

		textField = new JTextArea(5, 20);
	    textField.setLineWrap(true);
	    
	    JScrollPane areaScrollPane2 = new JScrollPane(textField);
	    areaScrollPane2.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
	    areaScrollPane2.setPreferredSize(new Dimension(300, 300));

	    gridConst.fill = GridBagConstraints.HORIZONTAL;
	    gridConst.gridy=0;
	    gridConst.gridwidth = 2;
	    this.add(areaScrollPane2,gridConst);
		    
		
		//textField = new JTextField(40);
	    //textField.addActionListener(this);    
	    //this.add(textField,gridConst);
	    
	    btnSend = new JButton("Send");
	    btnSend.setVerticalTextPosition(AbstractButton.CENTER);
	    btnSend.addActionListener(this);
	    //this.add(btnSend,gridConst);
	    
	    btnPlay = new JButton("Play");
	    btnPlay.setVerticalTextPosition(AbstractButton.CENTER);
	    btnPlay.addActionListener(this);
	    this.add(btnPlay,gridConst);
	    
	    
	    JRadioButton btnWave = new JRadioButton("Wave");
	    btnWave.setActionCommand("wave");
	    btnWave.setSelected(true);

	    JRadioButton btnNoise = new JRadioButton("Noise");
	    btnNoise.setActionCommand("noise");
	    
	    group = new ButtonGroup();
	    group.add(btnWave);
	    group.add(btnNoise);
	    
	   
	    
        JPanel radioPanel = new JPanel(new GridLayout(0, 1));
        radioPanel.add(btnWave);
        radioPanel.add(btnNoise);
        gridConst.gridy=1;
        this.add(radioPanel,gridConst);
	    
	    textArea = new JTextArea(5, 20);
	    textArea.setLineWrap(true);
	    
	    areaScrollPane = new JScrollPane(textArea);
	    areaScrollPane.setVerticalScrollBarPolicy(
	    		JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
	    //areaScrollPane.setPreferredSize(new Dimension(600, 300));

	    gridConst.fill = GridBagConstraints.HORIZONTAL;
	    gridConst.gridy=2;
	    gridConst.gridwidth = 3;
	    this.add(areaScrollPane,gridConst);


	    
	   
	    
	    try {
	    	MidiDevice.Info[] devices=MidiSystem.getMidiDeviceInfo();
	    	for(int i=0;i<devices.length;i++){
	    		textArea.append(devices[i].getName()+": "+devices[i].getDescription()+"\n");
	    		if(devices[i].getName().equalsIgnoreCase("SB Live! MIDI UART")&& devices[i].getDescription().equalsIgnoreCase("External MIDI Port")){
	    			midiDevice=MidiSystem.getMidiDevice(devices[i]);
	    			midiDevice.open();
	    			receiver=midiDevice.getReceiver();
	    			
	    			
	    			break;
	    		}
	    			
	    	}

		} catch (Exception e) {
			e.printStackTrace();
		    System.exit(1);
		}

	    
	}
	


	@Override
	public void actionPerformed(ActionEvent e) {
		if(e.getSource()==this.btnPlay){

			if(!this.textField.getText().equals(this.lastSend)){
				sendPatch();
				this.lastSend=this.textField.getText();
			}
			
			ShortMessage msg = new ShortMessage();
		     try {
	    	 int channel=0,note=60;
		    	 if(this.group.getSelection().getActionCommand().equals("noise")){
		    		 channel=3;
		    		 note=127;
		    	 }	
		    	msg.setMessage(ShortMessage.NOTE_ON, channel, note, 127);
				receiver.send(msg, 0);
			} catch (InvalidMidiDataException e1) {
				e1.printStackTrace();
			}
		}else if(e.getSource()==this.btnSend){

			sendPatch();
			this.lastSend=this.textField.getText();
						
		}
	}
	
	private void sendPatch(){
		byte[] b=parseCommands();
	     
		
		try {
			ShortMessage msg2 = new ShortMessage();
			msg2.setMessage(ShortMessage.PROGRAM_CHANGE, 0, 127,0 );
			receiver.send(msg2, 0);
			msg2.setMessage(ShortMessage.PROGRAM_CHANGE, 3, 127,0 );
			receiver.send(msg2, 0);
			
			
			SysexMessage msg=new SysexMessage();
			msg.setMessage(0xf0, b, b.length);
			receiver.send(msg, 0);
		} catch (InvalidMidiDataException e1) {
			e1.printStackTrace();
		}
		
		
	}
	
	private byte[] parseCommands() {
	
		//#define PATCH_END		0xff
		
		String[] data=this.textField.getText().split(",");
		byte[] b=new byte[data.length+1];

		
		for(int i=0;i<data.length;i++){
			Integer com=getCommand(data[i]);
			if(com!=null){
				b[i]=(byte)com.intValue();
			}else if(data[i].trim().equalsIgnoreCase("PATCH_END")){
				b[i]=(byte)0xff;
			}else if(data[i].indexOf("0x")!=-1){
				String newval=data[i].substring(data[i].indexOf("0x")+2).trim();
				b[i]=(byte) Integer.parseInt(newval,16);
			}else{
				b[i]=(byte) Integer.parseInt(data[i].trim(),10);
			}
			
		}
		b[b.length-1]=(byte) 0xf7;
		
		return b;
	}

	private Integer getCommand(String command){
		String[] patchCommands={"PC_ENV_SPEED","PC_NOISE_PARAMS","PC_WAVE","PC_NOTE_UP","PC_NOTE_DOWN","PC_NOTE_CUT","PC_NOTE_HOLD","PC_ENV_VOL","PC_PITCH","PC_TREMOLO_LEVEL","PC_TREMOLO_RATE"};
		for(int i=0;i<patchCommands.length;i++){
			if(command.equalsIgnoreCase(patchCommands[i].trim())){
				return new Integer(i);
			}
		}
		return null;
	}
    
}
