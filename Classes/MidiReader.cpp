#include "MidiReader.h"

#include "MidiFile.h"

#include <iostream>
#include <iomanip>
#include <assert.h>
#include <vector>

ParsedFile parse_attack_notes(const char* file_path)
{
  ParsedFile parsed_file;
  smf::MidiFile midifile;

  midifile.read(file_path);

  midifile.linkNotePairs();
  midifile.doTimeAnalysis();

  const int tpqn = midifile.getTicksPerQuarterNote();
  const int tracks = midifile.getTrackCount();

  parsed_file.ticks_per_quarter_note = tpqn;

  assert(tracks >= 2);

  double spt = 0.0;

  // Find initial Seconds Per Tick
  for (int track=0; track < tracks; track++)
  {
    bool found = false;

    for (int event=0; event < midifile[track].size(); event++)
    {
      double spt_curr = midifile[track][event].getTempoSPT(tpqn);
      if (spt_curr != -1)
      {
        spt = spt_curr;
        parsed_file.seconds_per_tick = spt_curr;
        found = true;
        break;
      }
    }

    if (found)
      break;
 }

  const size_t track = 1; // Track 1 is first track from tuxguitar

  for (int event=0; event < midifile[track].size(); event++) {
    const double spt_curr = midifile[track][event].getTempoSPT(tpqn);
    if (spt_curr != -1)
    {
      spt = spt_curr;
    }

    const bool note_on = midifile[track][event].isNoteOn() == 1;

    if (note_on)
    {
      const int tick = midifile[track][event].tick;
      const double start_time = spt*midifile[track][event].tick;
      const double dur = midifile[track][event].getDurationInSeconds();
      const int note_id = midifile[track][event][1];

      parsed_file.notes.emplace_back(start_time, dur, note_id, tick);
    }
  }

  return parsed_file;
}
