#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "MidiReader.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void update(float dt) override;

    void create_tab_sprite();

    std::vector<cocos2d::EventKeyboard::KeyCode> heldKeys;
    std::vector<cocos2d::EventKeyboard::KeyCode> releasedKeys;

    std::map<int, cocos2d::EventKeyboard::KeyCode> note_to_key;

    std::vector<Note> current_notes;

    std::vector<int> missed_notes;

    double accum_time = 0.0;
    int last_hit_index = -1;
};

#endif // __HELLOWORLD_SCENE_H__
