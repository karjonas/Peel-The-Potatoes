#include "HelloWorldScene.h"

#include "MidiReader.h"
#include "LevelScene.h"

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
  GlobalData global_data;

  {
    LevelData level;
    level.audio_file = "/home/jonas/Downloads/Untitled.wav";
    level.midi_file = "/home/jonas/Downloads/Untitled.mid";
    global_data.levels.push_back(level);
  }

  auto first_level = LevelScene::createScene(global_data);
  Director::getInstance()->replaceScene(first_level);
}
