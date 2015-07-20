#include "LevelScene.h"

#include "MidiReader.h"
#include "WinningScene.h"
#include "PreLevelScene.h"

USING_NS_CC;

#include "audio/include/SimpleAudioEngine.h"
#include <iostream>

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
  parsed_file = parse_attack_notes(global_data.levels[global_data.curr_level_idx].midi_file.c_str());
  current_notes = parsed_file.notes;

  // FIXME: Make sure notes are sorted by time

  this->scheduleUpdate();

  // creating a keyboard event listener
  auto listener = EventListenerKeyboard::create();
  listener->onKeyPressed = CC_CALLBACK_2(LevelScene::onKeyPressed, this);
  listener->onKeyReleased = CC_CALLBACK_2(LevelScene::onKeyReleased, this);

  _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

  note_to_key[35] = EventKeyboard::KeyCode::KEY_A;
  note_to_string[35] = "A";

  note_to_key[40] = EventKeyboard::KeyCode::KEY_D;
  note_to_string[40] = "D";

  auto audio = CocosDenshion::SimpleAudioEngine::getInstance();

  audio->playBackgroundMusic(global_data.levels[global_data.curr_level_idx].audio_file.c_str(), false);

  const Size visibleSize = Director::getInstance()->getVisibleSize();
  const float mid_h = visibleSize.height/2;
  const float mid_w = visibleSize.width/2;


  {
    auto sprite = cocos2d::Sprite::create("background.png");
    sprite->setPosition(cocos2d::Point(mid_w, mid_h));
    addChild(sprite, 1);
  }

  create_tab_sprite();

  {
    hero_sprite = cocos2d::Sprite::create("hero.png");
    hero_sprite->setPosition(cocos2d::Point(mid_w - 100, 250));
    addChild(hero_sprite, 1);
  }

  {
    potato_sprite = cocos2d::Sprite::create("potato.png");
    potato_sprite->setPosition(cocos2d::Point(mid_w + 100, 250));
    addChild(potato_sprite, 1);
  }

  {
    hero_health_label = Label::createWithTTF("100", "fonts/Marker Felt.ttf",32);
    hero_health_label->setPosition(cocos2d::Point(mid_w - 100, 250 + 100));
    addChild(hero_health_label, 1);
  }
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

  const size_t num_lines = max_song_length_secs/secs_per_quarter;

  for (size_t i = 0; i < num_lines; i++)
  {

    auto sprite = (i % 2 == 0) ? cocos2d::Sprite::create("line_tall.png") : cocos2d::Sprite::create("line_short.png") ;

    sprite->setPosition(cocos2d::Point(mid_w + i*secs_per_quarter*pixels_per_sec , 51));

    auto moveBy = MoveBy::create(max_song_length_secs, Vec2(-pixels_per_sec*max_song_length_secs, 0));

    sprite->runAction(moveBy);

    addChild(sprite, 1);
  }

  for (const Note& note : current_notes)
  {
    NoteSprite note_sprite;

    auto label = Label::createWithTTF(note_to_string[note.note_id], "fonts/Marker Felt.ttf",32);

    label->setPosition(cocos2d::Point(mid_w + note.start_time*pixels_per_sec , 50));

    auto moveBy = MoveBy::create(max_song_length_secs, Vec2(-pixels_per_sec*max_song_length_secs, 0));

    label->runAction(moveBy);

    addChild(label, 1);

    note_sprite.note = note;
    note_sprite.label = label;
    note_sprites.push_back(note_sprite);
  }

}

std::vector<int> LevelScene::get_current_note_sprite_indices() const
{
  std::vector<int> indices;

  size_t idx = 0;
  for (auto note_sprite : note_sprites)
  {
    if (note_sprite.note.start_time <= accum_time)
     indices.push_back(idx);
    else
      break;
    idx++;
  }

  return indices;
}

void LevelScene::prune_old_notes()
{
  const double time = accum_time;
  auto remove_it = std::remove_if(note_sprites.begin(), note_sprites.end(), [&](NoteSprite& ns)
  {
    if ((ns.note.start_time + ns.note.duration) < time)
    {
      if (!ns.has_hit)
      {
        std::cout << "note missed" << std::endl;
        player_health -= GlobalData::c_note_miss_damage;
        hero_health_label->setString(std::to_string(static_cast<int>(player_health)));

        const Size visibleSize = Director::getInstance()->getVisibleSize();
        const float mid_w = visibleSize.width/2;

        auto moveToAttack = MoveTo::create(0.1f, cocos2d::Point(mid_w + 50, 250));
        auto moveToReturn = MoveTo::create(0.1f, cocos2d::Point(mid_w + 100, 250));

        auto seq = Sequence::create(moveToAttack, moveToReturn, nullptr);
        potato_sprite->runAction(seq);

        auto tintTo0 = TintTo::create(0.1f, 255.0f, 0.0f, 0.0f);
        auto tintTo1 = TintTo::create(0.1f, 255.0f, 255.0f, 255.0f);
        auto seq_hero = Sequence::create(tintTo0, tintTo1, nullptr);

        hero_sprite->runAction(seq_hero);

        //missed_notes.push_back(note.idx);
      }
      ns.label->removeFromParentAndCleanup(true);

      std::cout << "--" << std::endl;

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
  accum_time += static_cast<double>(dt);

  prune_old_notes();

  const bool song_done = accum_time > 50;

  if (song_done)
  {
      GlobalData global_data_new = global_data;
      global_data_new.curr_level_idx++;

      auto audio = CocosDenshion::SimpleAudioEngine::getInstance();
      audio->stopBackgroundMusic(true);

      Scene* next_level;

      if (global_data_new.curr_level_idx >= global_data_new.levels.size())
        next_level = WinningScene::createScene(); // FIXME: Set Winning Screen
      else
        next_level = PreLevelScene::createScene(global_data_new);

      Director::getInstance()->replaceScene(next_level);
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
          std::cout << "HIT" << std::endl;
        }
        hit = true;
      }
    }

    if (!hit)
    {
      player_health -= GlobalData::c_hold_miss_damage_per_sec*static_cast<double>(dt);
      hero_health_label->setString(std::to_string(static_cast<int>(player_health)));

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
      hero_sprite->runAction(seq);

      auto tintTo0 = TintTo::create(0.1f, 255.0f, 0.0f, 0.0f);
      auto tintTo1 = TintTo::create(0.1f, 255.0f, 255.0f, 255.0f);
      auto seq_potato = Sequence::create(tintTo0, tintTo1, nullptr);

      potato_sprite->runAction(seq_potato);
  }

}


void LevelScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
