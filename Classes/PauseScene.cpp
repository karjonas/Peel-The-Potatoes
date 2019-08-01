#include "PauseScene.h"

USING_NS_CC;

Scene* PauseScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = PauseScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool PauseScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Layer::init())
    {
        return false;
    }

    { // creating a keyboard event listener
        auto listener = EventListenerKeyboard::create();
        listener->onKeyPressed = CC_CALLBACK_2(PauseScene::onKeyPressed, this);
        listener->onKeyReleased =
            CC_CALLBACK_2(PauseScene::onKeyReleased, this);
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener,
                                                                 this);
    }

    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const float mid_h = visibleSize.height / 2;
    const float mid_w = visibleSize.width / 2;

    {
        button_holder = cocos2d::Sprite::create();
        button_holder->setPosition(cocos2d::Point(mid_w, mid_h));
        addChild(button_holder, 1);
    }

    {
        float offset = 0.0;

        {
            auto label =
                Label::createWithTTF("EXIT", "fonts/Xolonium-Bold.otf", 32);
            label->setPosition(cocos2d::Point(0, -15));
            offset += label->getContentSize().height;
            button_holder->addChild(label, 1);
            exit_label = label;
        }

        {
            auto label =
                Label::createWithTTF("CONTINUE", "fonts/Xolonium-Bold.otf", 32);
            label->setPosition(cocos2d::Point(0, 15));
            offset += label->getContentSize().height;
            button_holder->addChild(label, 1);
            continue_label = label;
        }

        selected_label = continue_label;
        selected_label->setColor(cocos2d::Color3B(0, 255, 0));
    }

    return true;
}

// Implementation of the keyboard event callback function prototype
void PauseScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event) {}

void PauseScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
        Director::getInstance()->popScene();
    else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW ||
             keyCode == EventKeyboard::KeyCode::KEY_TAB)
    {
        selected_label->setColor(cocos2d::Color3B(255, 255, 255));

        if (selected_label == exit_label)
            selected_label = continue_label;
        else
            selected_label = exit_label;

        selected_label->setColor(cocos2d::Color3B(0, 255, 0));
    }
    else if (keyCode == EventKeyboard::KeyCode::KEY_KP_ENTER ||
             keyCode == EventKeyboard::KeyCode::KEY_ENTER)
    {
        if (selected_label == exit_label)
            Director::getInstance()->end();
        else if (selected_label == continue_label)
            Director::getInstance()->popScene();
    }
}

void PauseScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
