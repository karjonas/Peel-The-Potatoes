#ifndef __NOTE_SPRITE_H__
#define __NOTE_SPRITE_H__

#include "cocos2d.h"

#include "MidiReader.h"

struct NoteSprite
{
  Note note;
  cocos2d::Sprite* sprite = nullptr;
};

#endif // __NOTE_SPRITE_H__
