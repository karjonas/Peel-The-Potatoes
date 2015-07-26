#include "LevelScene.h"

#include "MidiReader.h"
#include "WinningScene.h"
#include "PreLevelScene.h"
#include "PauseScene.h"

USING_NS_CC;

#include "audio/include/SimpleAudioEngine.h"
#include "audio/include/AudioEngine.h"
#include <iostream>
#include <CCFileUtils.h>

const int MOVE_TAG = 1;
const int TINT_TAG = 2;

using namespace cocos2d::experimental;

Scene* LevelScene::createScene(GlobalData global_data)
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LevelScene::create();

    layer->post_init(global_data);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LevelScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    return true;
}

void LevelScene::post_init(GlobalData global_data)
{
  this->global_data = global_data;

  auto futils = FileUtils::getInstance();
  const std::string path = futils->fullPathForFilename(global_data.levels[global_data.curr_level_idx].midi_file);
  parsed_file = parse_attack_notes(path.c_str());
  current_notes = parsed_file.notes;

  song_end_time = parsed_file.notes.back().start_time + 5.0;

  // FIXME: Make sure notes are sorted by time

  this->scheduleUpdate();

  // creating a keyboard event listener
  auto listener = EventListenerKeyboard::create();
  listener->onKeyPressed = CC_CALLBACK_2(LevelScene::onKeyPressed, this);
  listener->onKeyReleased = CC_CALLBACK_2(LevelScene::onKeyReleased, this);

  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

  note_to_key[35] = EventKeyboard::KeyCode::KEY_H;
  note_to_string[35] = "H";
  note_to_offset_idx[35] = -1;

  note_to_key[36] = EventKeyboard::KeyCode::KEY_H;
  note_to_string[36] = "H";
  note_to_offset_idx[36] = -1;

  note_to_key[40] = EventKeyboard::KeyCode::KEY_D;
  note_to_string[40] = "D";
  note_to_offset_idx[40] = 0;

  note_to_key[38] = EventKeyboard::KeyCode::KEY_D;
  note_to_string[38] = "D";
  note_to_offset_idx[38] = 0;

  note_to_key[49] = EventKeyboard::KeyCode::KEY_S;
  note_to_string[49] = "S";
  note_to_offset_idx[49] = 1;

  note_to_key[45] = EventKeyboard::KeyCode::KEY_W;
  note_to_string[45] = "W";
  note_to_offset_idx[45] = 1;

  note_to_key[43] = EventKeyboard::KeyCode::KEY_E;
  note_to_string[43] = "E";
  note_to_offset_idx[43] = 1;

  audio_id = AudioEngine::play2d(global_data.levels[global_data.curr_level_idx].audio_file.c_str(), false, 1.0);

  const Size visibleSize = Director::getInstance()->getVisibleSize();
  const float mid_h = visibleSize.height/2;
  const float mid_w = visibleSize.width/2;

  {
    auto sprite = cocos2d::Sprite::create("pics/background.png");
    sprite->setPosition(cocos2d::Point(mid_w, mid_h));
    addChild(sprite, 1);
  }

  create_tab_sprite();

  {
    hero_sprite = cocos2d::Sprite::create("pics/hero.png");
    hero_sprite->setPosition(cocos2d::Point(mid_w - 100, 250));
    addChild(hero_sprite, 1);
  }

  {
    potato_sprite = cocos2d::Sprite::create("pics/potato.png");
    potato_sprite->setPosition(cocos2d::Point(mid_w + 100, 250));
    addChild(potato_sprite, 1);
  }

  {
    auto tex_cache = CCTextureCache::sharedTextureCache();
    heart_full = tex_cache->addImage("pics/heart_full.png");
    heart_empty = tex_cache->addImage("pics/heart_empty.png");
    heart_half = tex_cache->addImage("pics/heart_half.png");
  }

  if (godmode)
    player_health = 9999999;

  for (int i = 0; i < 10; i++)
    last_diffs.push_back(0.0);
}

void LevelScene::create_tab_sprite()
{
  const size_t num_notes = current_notes.size();

  note_sprites.reserve(num_notes);

  const int max_song_length_secs = 10*60;
  const int pixels_per_sec = 200;

  const Size visibleSize = Director::getInstance()->getVisibleSize();
  const float mid_w = visibleSize.width/2;

  // Fill lines
  const double secs_per_quarter =
      parsed_file.ticks_per_quarter_note * parsed_file.seconds_per_tick;

  const size_t num_lines = 10;

  const double width_in_pixels = secs_per_quarter*pixels_per_sec;

  {
    line_holder = cocos2d::Sprite::create("pics/line_tall.png");
    line_holder->setAnchorPoint(Vec2(0.5,0.5));
    line_holder->setPosition(mid_w, 51);
  
    addChild(line_holder, 1);
  
    auto moveByStart = MoveBy::create(2 * secs_per_quarter, Vec2(-2 * pixels_per_sec*secs_per_quarter, 0));
    auto moveToEnd = MoveTo::create(0.0f, Vec2(mid_w, 51));
    auto seq = Sequence::create(moveByStart, moveToEnd, nullptr);
  
    line_holder->runAction(RepeatForever::create(seq));
  }

  for (size_t i = 1; i < num_lines; i++)
  {
      auto sprite0 = (i % 2 == 0) ? cocos2d::Sprite::create("pics/line_tall.png") : cocos2d::Sprite::create("pics/line_short.png");
      auto sprite1 = (i % 2 == 0) ? cocos2d::Sprite::create("pics/line_tall.png") : cocos2d::Sprite::create("pics/line_short.png");

      const double start_x0 = (i*width_in_pixels);
      const double start_y0 = 0;

      const double start_x1 = (-static_cast<double>(i)*width_in_pixels);
      const double start_y1 = 0;

      sprite0->setAnchorPoint(Vec2(0, 0));
      sprite1->setAnchorPoint(Vec2(0, 0));

      sprite0->setPosition(cocos2d::Point(start_x0, start_y0));
      sprite1->setPosition(cocos2d::Point(start_x1, start_y1));

      line_holder->addChild(sprite0, 1);
      line_holder->addChild(sprite1, 1);

      lines.push_back(sprite0);
      lines.push_back(sprite1);
  }

  {
      notes_holder = cocos2d::Sprite::create();
      notes_holder->setAnchorPoint(Vec2(0.5, 0.5));
      notes_holder->setPosition(mid_w, 51);

      addChild(notes_holder, 1);

      auto moveByStart = MoveBy::create(secs_per_quarter, Vec2(-pixels_per_sec*secs_per_quarter, 0));
      auto seq = Sequence::create(moveByStart, nullptr);

      notes_holder->runAction(RepeatForever::create(seq));
  }

  for (const Note& note : current_notes)
  {
    NoteSprite note_sprite;

    auto label = Label::createWithTTF(note_to_string[note.note_id], "fonts/Xolonium-Bold.otf", 32);

    const int offset = note_to_offset_idx[note.note_id]*25;

    label->setPosition(cocos2d::Point(note.start_time*pixels_per_sec , offset));

    notes_holder->addChild(label, 1);

    note_sprite.note = note;
    note_sprite.label = label;
    note_sprites.push_back(note_sprite);
  }

  {
    hearts_holder = cocos2d::Sprite::create();
    hearts_holder->setAnchorPoint(Vec2(0.5, 0.5));
    hearts_holder->setPosition(10, visibleSize.height - 10);
    addChild(hearts_holder, 1);
  }

  for (int i = 0; i < 10; i++)
  {
    auto sprite = cocos2d::Sprite::create("pics/heart_full.png");
    sprite->setPosition(0 + i *16, 0);
    hearts_holder->addChild(sprite);
    hearts.push_back(sprite);
  }
  finished_note_sprites.reserve(note_sprites.size());
}

std::vector<int> LevelScene::get_current_note_sprite_indices() const
{
  std::vector<int> indices;

  size_t idx = 0;
  for (auto note_sprite : note_sprites)
  {
    if ((note_sprite.note.start_time - global_data.c_note_pre_leeway) <= accum_time_since_sync)
      indices.push_back(idx);
    else
      break;
    idx++;
  }

  return indices;
}

void LevelScene::update_health_bar()
{
  const int num_hearts = hearts.size();
  for (int i = 0; i < num_hearts; i++)
  {
    const double heart_health = i*10;
    if (player_health > (heart_health + 5))
      hearts[i]->setTexture(heart_full);
    else if (player_health <= heart_health)
      hearts[i]->setTexture(heart_empty);
    else
      hearts[i]->setTexture(heart_half);
  }
}

void LevelScene::prune_old_notes()
{
  const double time = accum_time_since_sync;
  auto remove_it = std::remove_if(note_sprites.begin(), note_sprites.end(), [&](NoteSprite& ns)
  {
    if ((ns.note.start_time + global_data.c_note_duration) < time)
    {
      if (!ns.has_hit)
      {
        player_health -= global_data.c_note_miss_damage;

        const Size visibleSize = Director::getInstance()->getVisibleSize();
        const float mid_w = visibleSize.width/2;

        auto moveToAttack = MoveTo::create(0.1f, cocos2d::Point(mid_w + 50, 250));
        auto moveToReturn = MoveTo::create(0.1f, cocos2d::Point(mid_w + 100, 250));

        auto seq = Sequence::create(moveToAttack, moveToReturn, nullptr);
        seq->setTag(MOVE_TAG);

        potato_sprite->stopAllActionsByTag(MOVE_TAG);
        potato_sprite->runAction(seq);

        auto tintTo0 = TintTo::create(0.1f, 255.0f, 0.0f, 0.0f);
        auto tintTo1 = TintTo::create(0.1f, 255.0f, 255.0f, 255.0f);
        auto seq_hero = Sequence::create(tintTo0, tintTo1, nullptr);
        seq_hero->setTag(TINT_TAG);

        hero_sprite->stopAllActionsByTag(TINT_TAG);
        hero_sprite->runAction(seq_hero);

        ns.label->setColor(cocos2d::Color3B(255,0,0));
      }

      finished_note_sprites.push_back(ns);

      old_notes.push_back(ns.label);

      return true;
    }
    return false;
  });

  note_sprites.erase(remove_it, note_sprites.end());
}

// Implementation of the keyboard event callback function prototype
void LevelScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
  if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end())
    heldKeys.push_back(keyCode);
}

void LevelScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
  heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
  releasedKeys.push_back(keyCode);
}

void LevelScene::update(float dt)
{
  AudioEngine::resume(audio_id);

  // Check if Esc pressed
  if (std::find(releasedKeys.begin(), releasedKeys.end(), EventKeyboard::KeyCode::KEY_ESCAPE) != releasedKeys.end())
  {
    AudioEngine::pause(audio_id);
    auto pauseScene = PauseScene::createScene();
    Director::getInstance()->pushScene(pauseScene);
    releasedKeys.clear();
    return;
  }

  AudioEngine::AudioState state = AudioEngine::getState(audio_id);
  double curr_time = AudioEngine::getCurrentTime(audio_id);
  accum_time_since_sync += static_cast<double>(dt);
  const double diff_now = abs(curr_time - accum_time_since_sync);

  last_diffs.pop_back();
  last_diffs.push_front(diff_now);

  double diff_avg = 0.0;
  const double num_diffs = static_cast<double>(last_diffs.size());
  for (double diff : last_diffs)
      diff_avg += diff;

  diff_avg /= num_diffs;

  if (state == AudioEngine::AudioState::PLAYING && curr_time > 0.0f && diff_avg > resync_threshold)
  {
    const int max_song_length_secs = 10 * 60;
    const int pixels_per_sec = 200;
    
    const double secs_per_quarter =
        parsed_file.ticks_per_quarter_note * parsed_file.seconds_per_tick;
    
    const size_t num_lines = lines.size();
    
    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const float mid_h = visibleSize.height / 2;
    const float mid_w = visibleSize.width / 2;
    
    const double moved_pixels = pixels_per_sec*curr_time;
    const double width_in_pixels = secs_per_quarter*pixels_per_sec;

    const int num_cycles = static_cast<int>(moved_pixels / (2*width_in_pixels));
    const double offset = moved_pixels - (2*width_in_pixels*num_cycles);

    {
        line_holder->setPosition(-offset + mid_w, 51);

        line_holder->stopAllActions();
        auto moveByStart = MoveBy::create(2 * secs_per_quarter, Vec2(-2 * pixels_per_sec*secs_per_quarter, 0));
        auto moveToEnd = MoveTo::create(0.0f, Vec2(-offset + mid_w, 51));
        auto seq = Sequence::create(moveByStart, moveToEnd, nullptr);

        line_holder->runAction(RepeatForever::create(seq));
    }

    notes_holder->setPosition(mid_w - moved_pixels, 51);

    last_diffs.clear();
    for (int i = 0; i < 10; i++)
        last_diffs.push_back(0.0);

    accum_time_since_sync = curr_time;

    log("Re-sync to audio");
  }

  diff_last = diff_now;

  prune_old_notes();

  update_health_bar();

  const bool song_done = player_health <= 0.0 || note_sprites.empty();

  if (song_done)
  {
      AudioEngine::setVolume(audio_id, 0.5);

      if (note_sprites.empty() && (accum_time_since_sync < song_end_time))
        return; // Wait some before next level

      AudioEngine::stop(audio_id);

      GlobalData global_data_new = global_data;

      if (player_health > 0.0)
      {
        global_data_new.curr_level_idx++;

        if (global_data_new.curr_level_idx < global_data_new.levels.size())
          global_data_new.curr_level_text = global_data_new.pre_level_text[global_data_new.curr_level_idx];
      }
      else
        global_data_new.curr_level_text = "Try Again";

      Scene* next_level;

      if (global_data_new.curr_level_idx >= global_data_new.levels.size())
        next_level = WinningScene::createScene();
      else
        next_level = PreLevelScene::createScene(global_data_new);

      Director::getInstance()->replaceScene(TransitionFade::create(0.5, next_level, Color3B(255,255,255)));
      return;
  }

  std::vector<int> current_indices = get_current_note_sprite_indices();

  bool attack = false;

  for (auto key : heldKeys)
  {
    bool hit = false;

    for (int idx : current_indices)
    {
      NoteSprite& note_sprite = note_sprites[idx];
      const Note& note = note_sprite.note;

      const EventKeyboard::KeyCode keyCode = note_to_key[note.note_id];

      if (keyCode == key)
      {
        if (!note_sprite.has_hit)
        {
          attack = true;
          note_sprite.has_hit = true;
          note_sprite.label->setColor(cocos2d::Color3B(0,255,0));
        }
        hit = true;
      }
    }

    if (!hit)
    {
      player_health -= global_data.c_hold_miss_damage_per_sec*static_cast<double>(dt);

      auto tintTo0 = TintTo::create(0.1f, 255.0f, 0.0f, 0.0f);
      auto tintTo1 = TintTo::create(0.1f, 255.0f, 255.0f, 255.0f);
      auto seq_hero = Sequence::create(tintTo0, tintTo1, nullptr);

      hero_sprite->runAction(seq_hero);
    }
  }

  if (attack)
  {
      const Size visibleSize = Director::getInstance()->getVisibleSize();
      const float mid_w = visibleSize.width/2;

      auto moveToAttack = MoveTo::create(0.1f, cocos2d::Point(mid_w - 50, 250));
      auto moveToReturn = MoveTo::create(0.1f, cocos2d::Point(mid_w - 100, 250));

      auto seq = Sequence::create(moveToAttack, moveToReturn, nullptr);
      seq->setTag(MOVE_TAG);

      hero_sprite->stopAllActionsByTag(MOVE_TAG);
      hero_sprite->runAction(seq);

      auto tintTo0 = TintTo::create(0.1f, 255.0f, 0.0f, 0.0f);
      auto tintTo1 = TintTo::create(0.1f, 255.0f, 255.0f, 255.0f);
      auto seq_potato = Sequence::create(tintTo0, tintTo1, nullptr);
      seq_potato->setTag(TINT_TAG);

      potato_sprite->stopAllActionsByTag(TINT_TAG);
      potato_sprite->runAction(seq_potato);
  }

  releasedKeys.clear();
}


void LevelScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
