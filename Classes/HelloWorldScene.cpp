#include "HelloWorldScene.h"

#include "MidiReader.h"

USING_NS_CC;

#include "audio/include/SimpleAudioEngine.h"
#include <iostream>

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    parsed_file = parse_attack_notes("/home/jonas/Downloads/Untitled.mid");
    current_notes = parsed_file.notes;

    // FIXME: Make sure notes are sorted by time

    this->scheduleUpdate();

    // creating a keyboard event listener
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    note_to_key[35] = EventKeyboard::KeyCode::KEY_A;
    note_to_string[35] = "A";

    note_to_key[40] = EventKeyboard::KeyCode::KEY_D;
    note_to_string[40] = "D";

    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();

    audio->playBackgroundMusic("/home/jonas/Downloads/Untitled.wav", true);

    create_tab_sprite();

    const Size visibleSize = Director::getInstance()->getVisibleSize();
    const float mid_h = visibleSize.height/2;
    const float mid_w = visibleSize.width/2;

    {
      hero_sprite = cocos2d::Sprite::create("hero.png");
      hero_sprite->setPosition(cocos2d::Point(mid_w - 100, mid_h));
      addChild(hero_sprite, 1);
    }

    {
      potato_sprite = cocos2d::Sprite::create("potato.png");
      potato_sprite->setPosition(cocos2d::Point(mid_w + 100, mid_h));
      addChild(potato_sprite, 1);
    }

    hero_health_label = Label::createWithTTF("100", "fonts/Marker Felt.ttf",32);
    hero_health_label->setPosition(cocos2d::Point(mid_w - 100, 300));
    addChild(hero_health_label, 1);

    return true;
}

void HelloWorld::create_tab_sprite()
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
    auto sprite = cocos2d::Sprite::create("line.png");

    sprite->setPosition(cocos2d::Point(mid_w + i*secs_per_quarter*pixels_per_sec , 50));

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

std::vector<int> HelloWorld::get_current_note_sprite_indices() const
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

void HelloWorld::prune_old_notes()
{
  const double time = accum_time;
  auto remove_it = std::remove_if(note_sprites.begin(), note_sprites.end(), [&](NoteSprite& ns)
  {
    if ((ns.note.start_time + ns.note.duration) < time)
    {
      if (!ns.has_hit)
      {
        std::cout << "note missed" << std::endl;
        player_health -= c_note_miss_damage;
        hero_health_label->setString(std::to_string(static_cast<int>(player_health)));

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
void HelloWorld::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
  if (std::find(heldKeys.begin(), heldKeys.end(), keyCode) == heldKeys.end())
    heldKeys.push_back(keyCode);
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
  heldKeys.erase(std::remove(heldKeys.begin(), heldKeys.end(), keyCode), heldKeys.end());
  releasedKeys.push_back(keyCode);
}

void HelloWorld::update(float dt)
{
  accum_time += static_cast<double>(dt);

  prune_old_notes();

  std::vector<int> current_indices = get_current_note_sprite_indices();

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
        note_sprite.has_hit = true;
        note_sprite.label->setColor(cocos2d::Color3B(0,255,0));
        std::cout << "HIT" << std::endl;

        hit = true;
      }
    }

    if (!hit)
    {
      player_health -= c_hold_miss_damage_per_sec*static_cast<double>(dt);
      hero_health_label->setString(std::to_string(static_cast<int>(player_health)));

      std::cout << "player_health" << player_health << std::endl;
      std::cout << "Miss" << std::endl;
    }
  }
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
