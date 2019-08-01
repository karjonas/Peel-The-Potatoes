#ifndef __NOTE_SPRITE_H__
#define __NOTE_SPRITE_H__

#include "cocos2d.h"

#include "MidiReader.h"

struct NoteSprite
{
    Note note;
    cocos2d::Label* label = nullptr;
    bool has_hit = false;
};

#endif // __NOTE_SPRITE_H__
