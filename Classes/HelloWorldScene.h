#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#include "MidiReader.h"
#include "NoteSprite.h"

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
    std::vector<int> get_current_note_sprite_indices() const;
    void prune_old_notes();


    std::vector<cocos2d::EventKeyboard::KeyCode> heldKeys;
    std::vector<cocos2d::EventKeyboard::KeyCode> releasedKeys;

    std::map<int, cocos2d::EventKeyboard::KeyCode> note_to_key;
    std::map<int, std::string> note_to_string;

    std::vector<Note> current_notes;

    std::vector<NoteSprite> note_sprites;


    std::vector<int> missed_notes;

    double accum_time = 0.0;

    double player_health = 100.0;

    static constexpr double c_note_miss_damage = 10.0;
    static constexpr double c_hold_miss_damage_per_sec = 10.0;
};

#endif // __HELLOWORLD_SCENE_H__
