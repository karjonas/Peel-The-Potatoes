#ifndef __LEVEL_SCENE_H__
#define __LEVEL_SCENE_H__

#include "cocos2d.h"

#include "MidiReader.h"
#include "NoteSprite.h"

#include "audio/include/AudioEngine.h"

#include <list>

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
  double c_note_duration = 0.2;
  double c_note_pre_leeway = 0.05;
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
    void update_health_bar();

    std::vector<cocos2d::EventKeyboard::KeyCode> heldKeys;
    std::vector<cocos2d::EventKeyboard::KeyCode> releasedKeys;

    ParsedFile parsed_file;

    cocos2d::Texture2D* heart_full = nullptr;
    cocos2d::Texture2D* heart_empty = nullptr;
    cocos2d::Texture2D* heart_half = nullptr;

    std::map<int, cocos2d::EventKeyboard::KeyCode> note_to_key;
    std::map<int, std::string> note_to_string;
    std::map<int, int> note_to_offset_idx;

    std::vector<Note> current_notes;

    std::vector<NoteSprite> note_sprites;
    std::vector<NoteSprite> finished_note_sprites;

    std::vector<cocos2d::Sprite*> lines;
    cocos2d::Sprite* line_holder = nullptr;
    cocos2d::Sprite* notes_holder = nullptr;
    cocos2d::Sprite* hearts_holder = nullptr;
    std::vector<cocos2d::Sprite*> hearts;

    cocos2d::Sprite* hero_sprite = nullptr;
    cocos2d::Sprite* potato_sprite = nullptr;

    std::vector<cocos2d::Label*> old_notes;

    double accum_time_since_sync = 0.0;

    double player_health = 100;
    double song_end_time = 0.0;
    double diff_last = 0.0;

    GlobalData global_data;

    int audio_id = -1;

    const bool godmode = false;
    const double resync_threshold = 0.2;

    std::list<double> last_diffs;
};

#endif // __LEVEL_SCENE_H__
