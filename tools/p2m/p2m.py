#usage: python p2m.py -i inputfile -s splitpoint

import os
import sys
import optparse
from midifile import *

class ZonedTrack(Track):
    # note = -1 => currently inactive
    def __init__(self, zone, note):
        Track.__init__(self)
        self.zone = zone
        self.note = note
        #for stats and debug
        self.minnote = 128
        self.maxnote = 0

    def AddEvent(self, time, event):
        self._events.append((time, event))
        if isinstance(event, NoteOnEvent):
            self.minnote = min(event.note, self.minnote)
            self.maxnote = max(event.note, self.maxnote)
    
class ZonedWriter(Writer):
    def __init__(self, split_note):
        Writer.__init__(self)
        #  0=C-1 up 127=G9
        i = 2 if split_note[1:2] == '#' else 1
        note = split_note[0:i].upper()
        octave = int(split_note[i:i+1])
        self.split = (octave + 1) * 12 + ['C','C#','D','D#','E','F','F#','G','G#','A','A#','B'].index(note)

    def ZoneOf(self, note):
        return "A" if note < self.split else "B"
    
    def AddTrack(self, note):
        new_track = ZonedTrack(self.ZoneOf(note), note)
        self._tracks.append(new_track)
        return new_track

    def FindFreeTrack(self, note):
        zone = self.ZoneOf(note)
        return next((t for t in self._tracks if t.zone == zone and t.note == -1), None)
    
    def TrackByNote(self, note):
        return next((t for t in self._tracks if t.note == note), None)
    
    #looks for an empty slot, if not found allocates a new track
    def NoteOn(self, d, e):
        if e.velocity == 0:
            self.NoteOff(d, e)
            return
        
        t = self.FindFreeTrack(e.note)
        if t is None:
            t = self.AddTrack(e.note)
    
        t.note = e.note
        t.AddEvent(d, e) 

    def NoteOff(self, d, e):
        t = self.TrackByNote(e.note)
        if not t is None:
            t.note = -1
            t.AddEvent(d, e) 
            
class Converter(object):

    #identify a playable track
    def PlayableTrack(self, t):
        for _, e in t:
            if isinstance(e, NoteOnEvent):
                return True                 
    
        return False
    
    def Convert(self, ifname, ofname, split_note):
        r = Reader()
        w = ZonedWriter(split_note)

        fin = file(ifname, 'rb')
        r.Read(fin)

        w.AddTrack(-1)
        trackname=''

        # given the limited scope of this script, tempo info tracks will be discarded
        for t in r.tracks:
            if self.PlayableTrack(t):     
                for d, e in t:
                    #voice specific events
                    if isinstance(e, ChannelEvent):
                        if isinstance(e, NoteOnEvent):
                            w.NoteOn(d, e)
                        elif isinstance(e, NoteOffEvent):
                            w.NoteOff(d, e)
                        elif isinstance(e, KeyAftertouchEvent):
                            w.TrackByNote(e.note).AddEvent(d, e) 
                        #any other channel event must be copied as is and propagated to the other tracks
                        else:
                            for t in w._tracks:
                                t.AddEvent(d, e)
                    #anything else except track names will be copied to the first track
                    else:
                        if isinstance(e, TrackNameEvent):
                            trackname = e.text
                        else:
                            w._tracks[0].AddEvent(d, e) 
        
        #stick the tracks of the same group together
        w._tracks.sort(None, lambda t: t.zone)
        for t in w._tracks:
            name = trackname + '_' + t.zone + str(w._tracks.index(t) + 1)
            t.AddEvent(1, TrackNameEvent(name))
            print name, t.minnote, t.maxnote
            
        w._ppq = r.ppq
        fout = file(ofname, 'wb')    
        w.Write(fout, format=1)
        fin.close()
        fout.close()
        
        return len(w._tracks)

if __name__ == '__main__':
    parser = optparse.OptionParser()
    parser.add_option(
      '-i',
      '--input_file',
      dest='input_file',
      default='Untitled.mid',
      help='input file FILE',
      metavar='FILE')
    parser.add_option(
      '-s',
      '--split_note',
      dest='split_note',
      default='G#9',
      help='split note letter/number format: C-1..G9',
      metavar='SPLIT')

    options, _ = parser.parse_args()

    c = Converter()
    ifname = options.input_file 
    ofname = os.path.splitext(ifname)[0] + '_multi.mid'
    i = c.Convert(ifname, ofname, options.split_note)

    sys.stdout.write(str(i) + ' Mono Tracks Generated into ' + ofname)