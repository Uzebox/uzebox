/*

  midiconv.cpp

  Copyright (c) 2017 Matt Pandina, Copyright (c) 2009 Alec Bourque
  All rights reserved.

  This file is part of the Uzebox (tm) project.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Options.h"
#include "MidiFile.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <stdlib.h>

using namespace std;

Options options;

void checkOptions(Options& opts);
void usage(const string& command);
void convertSong(const string& infile, const string& outfile);
void setEventTick(MidiEvent* mev, double tempo);
int writeInc(MidiFile& mf, const char* aFile, const char* varname);
int writeInc(MidiFile& mf, const string& aFile, const string& varname);
int writeInc(MidiFile& mf, ostream& out, const char* varname);
void debugPrint(MidiFile& mf, ostream& out);

int main(int argc, char** argv)
{
  cout << "Uzebox (tm) C++ MIDI converter 1.0" << endl;
  cout << "(c) 2017 Matt Pandina, (c) 2009 Alec Bourque" << endl;
  cout << "Built using: Midifile (c) 1999-2015, Craig Stuart Sapp. All rights reserved." << endl;
  cout << "This tool is released under the GNU GPL V3." << endl << endl;

  options.setOptions(argc, argv);
  checkOptions(options);
  convertSong(options.getArg(1), options.getArg(2));
  cout << "Done!" << endl;
  return 0;
}

void checkOptions(Options& opts)
{
  opts.define("h|help=b", "Display this help text");
  opts.define("f|factor=d:30.0", "Speed correction factor (double). Defaults to 30.");
  opts.define("v|varname=s:midisong", "Variable name used in the include file. Defaults to 'midisong'");
  opts.define("s|start=i:-1", "Force a loop start (specified in ticks). Any existing loop start in the input will be discarded.");
  opts.define("e|end=i:-1", "Force a loop end (specified in tick). Any existing loop end in the input will be discarded.");
  opts.define("d|debug=b", "Print debugging information");
  opts.define("1|no1=b", "Include note off events for channel 1");
  opts.define("2|no2=b", "Include note off events for channel 2");
  opts.define("3|no3=b", "Include note off events for channel 3");
  opts.define("4|no4=b", "Include note off events for channel 4");
  opts.define("5|no5=b", "Include note off events for channel 5");
  opts.process();

  if (opts.getBoolean("help") || opts.getArgCount() != 2) {
    usage(opts.getCommand());
    exit(0);
  }

  if ((opts.getInteger("start") == -1 && opts.getInteger("end") != -1) ||
      (opts.getInteger("start") != -1 && opts.getInteger("end") == -1)) {
    cerr << "Error: invalid command arguments. Both start and end must be specified (or omitted)." << endl;
    exit(1);
  } else if ((opts.getInteger("start") >= opts.getInteger("end")) &&
             (opts.getInteger("start") != -1 && opts.getInteger("end") != -1)) {
    cerr << "Error: invalid command arguments. When specified, start must be smaller than end." << endl;
    exit(1);
  }
}

void usage(const string& command)
{
  cout <<
    "                                                                     \n"
    "Converts a MIDI song in format 0 or 1 to a Uzebox MIDI stream        \n"
    "outputted as a C include file.                                       \n"
    "                                                                     \n"
    "Usage: " << command << " [OPTION]... INFILE OUTFILE                  \n"
    "                                                                     \n"
    "Options:                                                             \n"
    "   -h, --help     Display this help text and exit                    \n"
    "   -f, --factor   Speed correction factor (double). Defaults to 30.  \n"
    "   -v, --varname  Variable name used in the include file. Defaults   \n"
    "                  to 'midisong'                                      \n"
    "   -s, --start    Force a loop start (specified in ticks). Any       \n"
    "                  existing loop start in the input will be discarded.\n"
    "   -e, --end      Force a loop end (specified in ticks). Any existing\n"
    "                  loop end in the input will be discarded.           \n"
    "   -d, --debug    Print debugging information                        \n"
    "   -1, --no1      Include note off events for channel 1              \n"
    "   -2, --no2      Include note off events for channel 2              \n"
    "   -3, --no3      Include note off events for channel 3              \n"
    "   -4, --no4      Include note off events for channel 4              \n"
    "   -5, --no5      Include note off events for channel 5              \n"
       << endl;
}

#define CONTROLLER_TREMOLO 92
#define CONTROLLER_TREMOLO_RATE 100
#define CONTROLLER_VOL 7
#define CONTROLLER_EXPRESSION 11

void convertSong(const string& infile, const string& outfile)
{
  int status;
  MidiFile midifile;

  status = midifile.read(infile);
  if (status == 0) {
    cerr << "Error: could not read MIDI file '" << infile << "'." << endl;
    exit(1);
  }

  if (options.getBoolean("debug")) {
    cout << "----------------------------------------------------------------------------" << endl;
    cout << "Before processing, '" << infile << "' contains:" << endl;
    debugPrint(midifile, cout);
    cout << "----------------------------------------------------------------------------" << endl;
  }
  
  // if the MIDI is a type 1, convert it into a type 0
  midifile.sortTracks();
  midifile.joinTracks();

  // convert "8? ?? 40" MIDI messages into "9? ?? 00" messages
  for (int i = 0; i < midifile.getTrackCount(); i++)
    for (int j = 0; j < midifile.getEventCount(i); j++) {
      if (!midifile[i][j].isNoteOff())
        continue;
      midifile[i][j].setCommandNibble(0x90);
      midifile[i][j].setParameters(midifile[i][j][1], 0);
    }

  MidiFile newmidi;
  MidiEvent* mev;
  newmidi.setTicksPerQuarterNote(midifile.getTicksPerQuarterNote());

  int tempo = 60000000 / midifile.getTicksPerQuarterNote();

  for (int track = 0; track < midifile.getTrackCount(); track++)
    for (int i = 0; i < midifile[track].size(); i++) {
      mev = &midifile[track][i];

      if (mev->isMeta()) {
        if ((*mev)[1] == 0x06 && mev->getSize() != 4) {
          cerr << "Error: meta marker found that was not 1 character." << endl;
          exit(1);
        }
        if (mev->isTempo()) {
          tempo = mev->getTempoMicro();
        } else if ((*mev)[1] == 0x06 && options.getInteger("start") == -1) {
          // only include loop meta events if none were specified on the command line
          setEventTick(mev, tempo);
          newmidi[track].append(*mev);
        }
      } else {
        if (mev->getChannel() == 9)
          mev->setChannel(3);

        if (mev->isController()) {
          if ((*mev)[1] == CONTROLLER_VOL ||
              (*mev)[1] == CONTROLLER_EXPRESSION ||
              (*mev)[1] == CONTROLLER_TREMOLO ||
              (*mev)[1] == CONTROLLER_TREMOLO_RATE) {
            setEventTick(mev, tempo);
            newmidi[track].append(*mev);
          }
        } else if (((*mev)[0] & 0x90) && (*mev)[2] == 0x00) {
          int chan = mev->getChannel();
          if ((options.getBoolean("no1") && chan == 0) ||
              (options.getBoolean("no2") && chan == 1) ||
              (options.getBoolean("no3") && chan == 2) ||
              (options.getBoolean("no4") && chan == 3) ||
              (options.getBoolean("no5") && chan == 4)) {
            setEventTick(mev, tempo);
            newmidi[track].append(*mev);
          }
        } else if (mev->isNoteOn() || mev->getCommandNibble() == 0xC0) {
          setEventTick(mev, tempo);
          newmidi[track].append(*mev);
        }
      }
    }

  // add looping meta events if required
  if (options.getInteger("start") != -1) {
    MidiEvent metaS;
    vector<uchar> dataS = {0xFF, 0x06, 0x01, 0x53};
    metaS.tick = options.getInteger("start");
    metaS.track = 0;
    metaS.setMessage(dataS);
    setEventTick(&metaS, tempo);
    newmidi[0].append(metaS);

    MidiEvent metaE;
    vector<uchar> dataE = {0xFF, 0x06, 0x01, 0x45};
    metaE.tick = options.getInteger("end");
    metaE.track = 0;
    metaE.setMessage(dataE);
    setEventTick(&metaE, tempo);
    newmidi[0].append(metaE);

    newmidi.sortTracks();
  }

  if (options.getBoolean("debug")) {
    cout << "After processing, '" << outfile << "' will contain:" << endl;
    debugPrint(newmidi, cout);
    cout << "----------------------------------------------------------------------------" << endl;
  }

  // write it out as a C include file
  writeInc(newmidi, outfile, options.getString("varname"));
}

void setEventTick(MidiEvent* mev, double tempo)
{
  static int tickDiff = 0;
  static bool first = true;

  if (first && mev->tick != 0)
    tickDiff = mev->tick;

  double scaled = mev->tick - tickDiff;
  double factor = options.getDouble("factor") / (60000000.0 / tempo);
  scaled *= factor;
  mev->tick = (int)scaled;
  first = false;
}

int writeInc(MidiFile& mf, const char* aFile, const char* varname) {
  fstream output(aFile, ios::out);
  if (!output.is_open()) {
    cerr << "Error: could not write: " << aFile << endl;
    return 0;
  }
  cout << "Outputting file: " << aFile << endl;
  int rwstatus = writeInc(mf, output, varname);
  output.close();
  return rwstatus;
}

int writeInc(MidiFile& mf, const string& aFile, const string& varname) {
  return writeInc(mf, aFile.data(), varname.data());
}

int writeInc(MidiFile& mf, ostream& out, const char* varname) {
  stringstream tempstream;
  mf.write(tempstream);
  int value = 0;
  int len = (int)tempstream.str().length();
  int wordcount = 1;
  out << "const char " << varname << "[] PROGMEM = {" << endl << "  ";

  // save default formatting
  ios init(0);
  init.copyfmt(out);

  // skip the midi header
  for (int i = 22; i < len; i++) {
    value = (unsigned char)tempstream.str()[i];
    out << "0x" << hex << setw(2) << setfill('0') << right << value;
    if (i < len - 1)
      out << "," << (wordcount % 12 ? " " : "\n  ");
    wordcount++;
  }
  out << '\n';

  // restore default formatting
  out.copyfmt(init);

  out << "};" << endl;
  out << "/* PROGMEM usage: " << len - 22 << " */" << endl;
  cout << "Size: " << len - 22 << " bytes" << endl;
  return 1;
}

void debugPrint(MidiFile& mf, ostream& out)
{
  // save default formatting
  ios init(0);
  init.copyfmt(out);

  int tracks = mf.getTrackCount();
  out << "TPQ: " << mf.getTicksPerQuarterNote() << endl;
  if (tracks > 1)
    out << "TRACKS: " << tracks << endl;
  for (int track = 0; track < tracks; track++) {
    if (tracks > 1)
      out << endl << "Track " << track << endl;
    for (int event = 0; event < mf[track].size(); event++) {
      out << dec << mf[track][event].tick;
      out << '\t' << hex;
      for (unsigned int i = 0; i < mf[track][event].size(); i++)
        out << setw(2) << setfill('0') << right << (int)mf[track][event][i] << ' ';
      out << endl;
    }
  }

  // restore default formatting
  out.copyfmt(init);
}
