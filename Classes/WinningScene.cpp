#include "WinningScene.h"

USING_NS_CC;

Scene* WinningScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = WinningScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool WinningScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    { // creating a keyboard event listener
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed =
            CC_CALLBACK_2(WinningScene::onKeyPressed, this);
        listener->onKeyReleased =
            CC_CALLBACK_2(WinningScene::onKeyReleased, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener,
                                                                 this);
    }

    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const float mid_h = visibleSize.height / 2;
    const float mid_w = visibleSize.width / 2;

    {
        auto label = Label::createWithTTF("Congratulations!",
                                          "fonts/Xolonium-Bold.otf", 32);
        label->setPosition(cocos2d::Point(mid_w, mid_h + 15));
        addChild(label, 1);
    }
    {
        auto label = Label::createWithTTF("All potatoes are peeled!",
                                          "fonts/Xolonium-Bold.otf", 32);
        label->setPosition(cocos2d::Point(mid_w, mid_h - 15));
        addChild(label, 1);
    }
    return true;
}

// Implementation of the keyboard event callback function prototype
void WinningScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {}

void WinningScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE ||
        keyCode == EventKeyboard::KeyCode::KEY_SPACE ||
        keyCode == EventKeyboard::KeyCode::KEY_ENTER)
        Director::getInstance()->end();
}

void WinningScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
