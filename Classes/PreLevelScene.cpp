#include "PreLevelScene.h"

USING_NS_CC;

Scene* PreLevelScene::createScene(GlobalData global_data)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = PreLevelScene::create();

    layer->post_init(global_data);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool PreLevelScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    { // creating a keyboard event listener
      auto listener = EventListenerKeyboard::create();
      listener->onKeyPressed = CC_CALLBACK_2(PreLevelScene::onKeyPressed, this);
      listener->onKeyReleased = CC_CALLBACK_2(PreLevelScene::onKeyReleased, this);
      _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

    return true;
}

void PreLevelScene::post_init(GlobalData global_data)
{
  this->global_data = global_data;

  const Size visibleSize = Director::getInstance()->getVisibleSize();
  const float mid_h = visibleSize.height/2;
  const float mid_w = visibleSize.width/2;

  {
    auto level_label =  Label::createWithTTF(global_data.curr_level_text, "fonts/Marker Felt.ttf",32);
    level_label->setPosition(mid_w,mid_h);
    addChild(level_label, 1);
  }
}

// Implementation of the keyboard event callback function prototype
void PreLevelScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{

}

void PreLevelScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
  Scene* next_level = LevelScene::createScene(global_data);
  Director::getInstance()->replaceScene(TransitionFade::create(0.5, next_level, Color3B(255,255,255)));
}

void PreLevelScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
