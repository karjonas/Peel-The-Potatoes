#ifndef __MIDI_READER_H__
#define __MIDI_READER_H__

#include <vector>

struct Note
{
    Note(double start_time_in, double duration_in, int note_id_in, int idx_in)
        : start_time(start_time_in)
        , duration(duration_in)
        , note_id(note_id_in)
        , idx(idx_in)
    {}

    double start_time;
    double duration;
    int note_id;
    int idx;
};

std::vector<Note> parse_attack_notes(const char* file_path);

#endif // __MIDI_READER_H__
