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

    return true;
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

  if (!current_notes.empty())
  {
    const Note& next_note = current_notes.front();

    const EventKeyboard::KeyCode keyCode = note_to_key[next_note.note_id];

    if (next_note.start_time <= accum_time)
    {
      std::cout << next_note.note_id << std::endl;

      if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) != heldKeys.end())
      {
        last_hit_index = next_note.idx;
        std::cout << "HIT" << std::endl;
      }
    }

    if ((next_note.start_time + next_note.duration) <= accum_time)
    {
      if (last_hit_index == -1)
        missed_notes.push_back(next_note.idx);

      current_notes.erase(current_notes.begin());
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
