package org.uzebox.tools.converters.midi;

/***************************************************************************/
/*                                                                         */
/* (c) Copyright IBM Corp. 2001  All rights reserved.                      */
/*                                                                         */
/* This sample program is owned by International Business Machines         */
/* Corporation or one of its subsidiaries ("IBM") and is copyrighted       */
/* and licensed, not sold.                                                 */
/*                                                                         */
/* You may copy, modify, and distribute this sample program in any         */
/* form without payment to IBM, for any purpose including developing,      */
/* using, marketing or distributing programs that include or are           */
/* derivative works of the sample program.                                 */
/*                                                                         */
/* The sample program is provided to you on an "AS IS" basis, without      */
/* warranty of any kind.  IBM HEREBY  EXPRESSLY DISCLAIMS ALL WARRANTIES,  */
/* EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.     */
/* Some jurisdictions do not allow for the exclusion or limitation of      */
/* implied warranties, so the above limitations or exclusions may not      */
/* apply to you.  IBM shall not be liable for any damages you suffer as    */
/* a result of using, modifying or distributing the sample program or      */
/* its derivatives.                                                        */
/*                                                                         */
/* Each copy of any portion of this sample program or any derivative       */
/* work,  must include the above copyright notice and disclaimer of        */
/* warranty.                                                               */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/*                                                                         */
/* This file accompanies the article "Understanding and using Java MIDI    */
/* audio." This article was published in the Special Edition 2001 issue    */
/* of the IBM DeveloperToolbox Technical Magazine at                       */
/* http://www.developer.ibm.com/devcon/mag.htm.                            */
/*                                                                         */
/***************************************************************************/

//------------------------------------------------------------------------
// File Name:     MessageInfo 
// Description:   Converts MidiMessages to meaningful English strings.
//------------------------------------------------------------------------

import java.io.PrintStream;
import javax.sound.midi.*;

/**
 * MessageInfo Converts MidiMessages to meaningful English strings.
 * 
 * Much of the MidiMessage information comes from information provided by
 * Matthias Pfisterer's Java Examples at
 * http://rupert.informatik.uni-stuttgart.de/~pfistere/jsexamples
 */
public class MessageInfo {
	public static String[] keyNames = { "C", "C#", "D", "D#", "E", "F", "F#",
			"G", "G#", "A", "A#", "B" };

	public static String[] keySignatures = { "Cb", "Gb", "Db", "Ab", "Eb",
			"Bb", "F", "C", "G", "D", "A", "E", "B", "F#", "C#" };

	/** Converts the given MidiMessage to a String. */
	public static String toString(MidiMessage message) {
		if (message instanceof ShortMessage)
			return toString((ShortMessage) message);
		else if (message instanceof SysexMessage)
			return toString((SysexMessage) message);
		else if (message instanceof MetaMessage)
			return toString((MetaMessage) message);
		return "unknown midi message " + message;
	} // toString( MidiMessage )

	/** Converts the given ShortMessage to a String. */
	public static String toString(ShortMessage message) {
		String strMessage = null;
		switch (message.getCommand()) {
		case ShortMessage.NOTE_OFF:
			strMessage = "note off " + getKeyName(message.getData1())
					+ ", velocity: " + message.getData2();
			break;
		case ShortMessage.NOTE_ON:
			strMessage = "note " + getKeyName(message.getData1())
					+ " on velocity: " + message.getData2();
			break;
		case ShortMessage.POLY_PRESSURE:
			strMessage = "polyphonic key pressure "
					+ getKeyName(message.getData1()) + " pressure: "
					+ message.getData2();
			break;
		case ShortMessage.CONTROL_CHANGE:
			strMessage = "control change " + message.getData1() + " value: "
					+ message.getData2();
			break;
		case ShortMessage.PROGRAM_CHANGE:
			strMessage = "program change " + message.getData1();
			break;
		case ShortMessage.CHANNEL_PRESSURE:
			strMessage = "key pressure " + getKeyName(message.getData1())
					+ " pressure: " + message.getData2();
			break;
		case ShortMessage.PITCH_BEND:
			strMessage = "pitch bend "
					+ get14bitValue(message.getData1(), message.getData2());
			break;
		case 0xF0:
			switch (message.getChannel()) {
			case 0x0:
				strMessage = "System Exclusive (should not be in ShortMessage)";
				break;
			case 0x1:
				strMessage = "Undefined";
				break;
			case 0x2:
				strMessage = "Song Position: "
						+ get14bitValue(message.getData1(), message.getData2());
				break;
			case 0x3:
				strMessage = "Song Select: " + message.getData1();
				break;
			case 0x4:
				strMessage = "Undefined";
				break;
			case 0x5:
				strMessage = "Undefined";
				break;
			case 0x6:
				strMessage = "Tune Request";
				break;
			case 0x7:
				strMessage = "end of SysEx (should not be in ShortMessage)";
				break;
			case 0x8:
				strMessage = "Timing clock";
				break;
			case 0x9:
				strMessage = "Undefined";
				break;
			case 0xA:
				strMessage = "Start";
				break;
			case 0xB:
				strMessage = "Continue";
				break;
			case 0xC:
				strMessage = "Stop";
				break;
			case 0xD:
				strMessage = "Undefined";
				break;
			case 0xE:
				strMessage = "Active Sensing";
				break;
			case 0xF:
				strMessage = "System Reset";
				break;
			} // switch
			break;

		default:
			strMessage = "unknown message: status = " + message.getStatus()
					+ ", byte1 = " + message.getData1() + ", byte2 = "
					+ message.getData2();
			break;
		} // switch

		if (message.getCommand() != 0xF0) {
			int nChannel = message.getChannel() + 1;
			String strChannel = "channel " + nChannel + ": ";
			strMessage = strChannel + strMessage;
		}
		return strMessage;
	} // toString( ShortMessage )

	/** Converts the given SysexMessage to a String. */
	public static String toString(SysexMessage message) {
		byte[] abData = message.getData();
		String strMessage = null;
		if (message.getStatus() == SysexMessage.SYSTEM_EXCLUSIVE) {
			strMessage = "Sysex message: F0" + MessageInfo.getHexString(abData);
		} else if (message.getStatus() == SysexMessage.SPECIAL_SYSTEM_EXCLUSIVE) {
			strMessage = "Special Sysex message (F7):"
					+ MessageInfo.getHexString(abData);
		}
		return strMessage;
	} // toString( SysexMessage message )

	/** Converts the given MetaMessage to a String. */
	public static String toString(MetaMessage message) {
		byte[] abData = message.getData();
		int nDataLength = message.getLength();
		String strMessage = null;
		switch (message.getType()) {
		case 0:
			int nSequenceNumber = abData[0] * 256 + abData[1];
			strMessage = "Sequence Number: " + nSequenceNumber;
			break;
		case 1:
			String strText = new String(abData, 0, nDataLength);
			strMessage = "Text Event: " + strText;
			break;
		case 2:
			// String strCopyrightText = new String(abData, 0, nDataLength);
			// strMessage = "Copyright Notice: " + strCopyrightText;
			break;
		case 3:
			String strTrackName = new String(abData);
			strMessage = "Sequence/Track Name: " + strTrackName;
			break;
		case 4:
			String strInstrumentName = new String(abData);
			strMessage = "Instrument Name: " + strInstrumentName;
			break;
		case 5:
			String strLyrics = new String(abData);
			strMessage = "Lyric: " + strLyrics;
			break;
		case 6:
			String strMarkerText = new String(abData);
			strMessage = "Marker: " + strMarkerText;
			break;
		case 7:
			String strCuePointText = new String(abData);
			strMessage = "Cue Point: " + strCuePointText;
			break;
		case 0x20:
			int nChannelPrefix = abData[0];
			strMessage = "MIDI Channel Prefix: " + nChannelPrefix;
			break;
		case 0x2F:
			strMessage = "end of track";
			break;
		case 0x51:
			int nTempo = signedByteToUnsigned(abData[0]) * 65536
					+ signedByteToUnsigned(abData[1]) * 256
					+ signedByteToUnsigned(abData[2]);
			// TDebug.out("tempo (us/quarter note): " + nTempo);
			strMessage = "Set Tempo (us/quarter note): " + nTempo;
			break;
		case 0x54:
			strMessage = "SMTPE Offset: " + abData[0] + ":" + abData[1] + ":"
					+ abData[2] + "." + abData[3] + "." + abData[4];
			break;
		case 0x58:
			strMessage = "Time Signature: " + abData[0] + "/"
					+ (1 << abData[1]) + ", MIDI clocks per metronome tick: "
					+ abData[2] + ", 1/32 per 24 MIDI clocks: " + abData[3];
			break;
		case 0x59:
			String strGender = (abData[1] == 1) ? "minor" : "major";
			strMessage = "Key Signature: " + keySignatures[abData[0] + 7] + " "
					+ strGender;
			break;
		case 0x7F:
			String strDataDump = "";
			strMessage = "Sequencer-Specific Meta event: " + strDataDump;
			break;
		default:
			String strUnknownDump = "";
			strMessage = "unknown Meta event: " + strUnknownDump;
			break;
		}
		return strMessage;
	}

	/** Converts a given number to a note and a key. */
	public static String getKeyName(int nKeyNumber) {
		if (nKeyNumber > 127)
			return "illegal value";
		int nNote = nKeyNumber % 12;
		int nOctave = nKeyNumber / 12;
		return keyNames[nNote] + (nOctave - 1);
	} // getKeyName

	public static int get14bitValue(int lowPart, int highPart) {
		return (lowPart & 0x7F) | ((highPart & 0x7F) << 7);
	}

	/** Converts a a signed byte to unsigned. */
	public static int signedByteToUnsigned(byte b) {
		if (b >= 0)
			return (int) b;
		return 256 + (int) b;
	}

	public static String getHexString(byte[] aByte) {
		StringBuffer buf = new StringBuffer(aByte.length * 3 + 2);
		for (int i = 0; i < aByte.length; i++) {
			buf.append(' ');
			byte bhigh = (byte) ((aByte[i] & 0xf0) >> 4);
			buf.append((char) (bhigh > 9 ? bhigh + 'A' - 10 : bhigh + '0'));
			byte blow = (byte) (aByte[i] & 0x0f);
			buf.append((char) (blow > 9 ? blow + 'A' - 10 : blow + '0'));
		}
		return new String(buf);
	}
} // class MessageInfo
