#ifndef __PRELEVEL_SCENE_H__
#define __PRELEVEL_SCENE_H__

#include "LevelScene.h"

#include "cocos2d.h"

class PreLevelScene : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(GlobalData global_data);

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    void post_init(GlobalData global_data);

    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(PreLevelScene);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    GlobalData global_data;
    bool replace_scene = false;
};

#endif // __PRELEVEL_SCENE_H__
