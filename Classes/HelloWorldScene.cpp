#include "HelloWorldScene.h"

#include "MidiReader.h"
#include "PreLevelScene.h"

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
  auto audio = CocosDenshion::SimpleAudioEngine::getInstance();

  GlobalData global_data;

  {
    LevelData level;
    level.audio_file = "Ac Dc - Back In Black (Pro).wav";
    level.midi_file = "Ac Dc - Back In Black (Pro).mid";
    global_data.levels.push_back(level);
    global_data.pre_level_text.push_back("Kill the Potatoes!");
    global_data.curr_level_text = global_data.pre_level_text[0];
  }

  {
    LevelData level;
    level.audio_file = "Ac Dc - Highway To Hell (Pro).wav";
    level.midi_file = "Ac Dc - Highway To Hell (Pro).mid";
    global_data.levels.push_back(level);
    global_data.pre_level_text.push_back("The potatoes are resilient!");
  }

  {
    LevelData level;
    level.audio_file = "Kiss - Heaven's On Fire (Pro).wav";
    level.midi_file = "Kiss - Heaven's On Fire (Pro).mid";
    global_data.levels.push_back(level);
    global_data.pre_level_text.push_back("All potatoes must fall!");
  }

  for (auto& level : global_data.levels)
    audio->preloadBackgroundMusic(level.audio_file.c_str());

  auto first_level = PreLevelScene::createScene(global_data);
  Director::getInstance()->replaceScene(first_level);
}
