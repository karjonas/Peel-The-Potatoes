#include "HelloWorldScene.h"

#include "MidiReader.h"
#include "PreLevelScene.h"

USING_NS_CC;

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

    { // creating a keyboard event listener
      auto listener = EventListenerKeyboard::create();
      listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
      listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
      _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

    this->scheduleUpdate();

    return true;
}

// Implementation of the keyboard event callback function prototype
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
}

void HelloWorld::update(float dt)
{
  GlobalData global_data;

  {
    LevelData level;
    level.audio_file = "songs/Ac Dc - Back In Black.mp3";
    level.midi_file = "songs/Ac Dc - Back In Black.mid";
    global_data.pre_level_text.push_back("Peel the Potatoes!");

    level.key_to_note[EventKeyboard::KeyCode::KEY_H] = 35;
    level.key_to_note[EventKeyboard::KeyCode::KEY_D] = 40;
    global_data.levels.push_back(level);
  }

  {
    LevelData level;
    level.audio_file = "songs/Ac Dc - Highway To Hell.mp3";
    level.midi_file = "songs/Ac Dc - Highway To Hell.mid";
    global_data.pre_level_text.push_back("The potatoes are resilient!");
    level.key_to_note[EventKeyboard::KeyCode::KEY_H] = 36;
    level.key_to_note[EventKeyboard::KeyCode::KEY_D] = 40;
    global_data.levels.push_back(level);
  }

  {
    LevelData level;
    level.audio_file = "songs/Kiss - Heavens On Fire.mp3";
    level.midi_file = "songs/Kiss - Heavens On Fire.mid";
    global_data.pre_level_text.push_back("All potatoes must fall!");
    level.key_to_note[EventKeyboard::KeyCode::KEY_H] = 35;
    level.key_to_note[EventKeyboard::KeyCode::KEY_D] = 38;

    level.key_to_note[EventKeyboard::KeyCode::KEY_S] = 49;
    level.key_to_note[EventKeyboard::KeyCode::KEY_W] = 45;
    level.key_to_note[EventKeyboard::KeyCode::KEY_E] = 43;
    global_data.levels.push_back(level);
  }

  global_data.curr_level_text = global_data.pre_level_text[0];

  auto first_level = PreLevelScene::createScene(global_data);
  Director::getInstance()->replaceScene(first_level);
}
