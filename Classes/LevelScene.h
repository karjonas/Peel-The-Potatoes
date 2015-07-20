#ifndef __LEVEL_SCENE_H__
#define __LEVEL_SCENE_H__

#include "cocos2d.h"

#include "MidiReader.h"
#include "NoteSprite.h"

struct LevelData
{
  std::string pre_level_text = "";
  std::string audio_file;
  std::string midi_file;
};

struct GlobalData
{
  std::vector<LevelData> levels;
  std::vector<std::string> pre_level_text;
  std::string curr_level_text;
  size_t curr_level_idx = 0;

  double c_note_miss_damage = 10.0;
  double c_hold_miss_damage_per_sec = 10.0;
  double c_note_duration = 0.35;
  double c_note_pre_leeway = 0.1;
};


class LevelScene : public cocos2d::Layer
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
    CREATE_FUNC(LevelScene);

    void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

    void update(float dt) override;

    void create_tab_sprite();
    std::vector<int> get_current_note_sprite_indices() const;
    void prune_old_notes();


    std::vector<cocos2d::EventKeyboard::KeyCode> heldKeys;
    std::vector<cocos2d::EventKeyboard::KeyCode> releasedKeys;

    ParsedFile parsed_file;

    std::map<int, cocos2d::EventKeyboard::KeyCode> note_to_key;
    std::map<int, std::string> note_to_string;
    std::map<int, int> note_to_offset_idx;

    std::vector<Note> current_notes;

    std::vector<NoteSprite> note_sprites;

    cocos2d::Sprite* hero_sprite = nullptr;
    cocos2d::Sprite* potato_sprite = nullptr;

    std::vector<cocos2d::Label*> old_notes;

    double accum_time = 0.0;

    double player_health = 100000.0;

    double song_end_time = 0.0;

    GlobalData global_data;
};

#endif // __LEVEL_SCENE_H__
