#include "HelloWorldScene.h"

#include "MidiReader.h"

USING_NS_CC;

#include "audio/include/SimpleAudioEngine.h"
#include <iostream>

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    current_notes = parse_attack_notes("/home/jonas/Downloads/Untitled.mid");

    this->scheduleUpdate();

    // creating a keyboard event listener
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    note_to_key[35] = EventKeyboard::KeyCode::KEY_SPACE;

    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();

    audio->playBackgroundMusic("/home/jonas/Downloads/Untitled.wav", true);

    create_tab_sprite();


    return true;
}

void HelloWorld::create_tab_sprite()
{
  const size_t num_notes = current_notes.size();

  note_sprites.reserve(num_notes);

  const int max_song_length_secs = 10*60;
  const int pixels_per_sec = 200;

  for (const Note& note : current_notes)
  {
    NoteSprite note_sprite;

    cocos2d::Sprite* sprite = cocos2d::Sprite::create("/home/jonas/Downloads/Sprites/flatDark/flatDark00.png");

    sprite->setPosition(cocos2d::Point(note.start_time*pixels_per_sec , 50));

    auto moveBy = MoveBy::create(max_song_length_secs, Vec2(-pixels_per_sec*max_song_length_secs, 0));

    sprite->runAction(moveBy);

    addChild(sprite, 1);

    note_sprite.note = note;
    note_sprite.sprite = sprite;
    note_sprites.push_back(note_sprite);
  }
}

// Implementation of the keyboard event callback function prototype
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
  if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end())
    heldKeys.push_back(keyCode);
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
  heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
  releasedKeys.push_back(keyCode);
}

void HelloWorld::update(float dt)
{
  accum_time += static_cast<double>(dt);

  if (!note_sprites.empty())
  {
    const NoteSprite& next_note = note_sprites.front();
    const Note& note = next_note.note;

    const EventKeyboard::KeyCode keyCode = note_to_key[note.note_id];

    if (note.start_time <= accum_time)
    {
      std::cout << note.note_id << std::endl;

      if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) != heldKeys.end())
      {
        last_hit_index = note.idx;
        std::cout << "HIT" << std::endl;
      }
    }

    if ((note.start_time + note.duration) <= accum_time)
    {
      if (last_hit_index == -1)
        missed_notes.push_back(note.idx);

      next_note.sprite->removeFromParentAndCleanup(true);
      note_sprites.erase(note_sprites.begin());
      last_hit_index = -1;

      std::cout << "--" << std::endl;
    }


  }
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
