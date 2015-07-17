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
    
    current_notes = parse_attack_notes("/home/jonas/Downloads/Untitled.mid");

    // FIXME: Make sure notes are sorted by time

    this->scheduleUpdate();

    // creating a keyboard event listener
    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    note_to_key[35] = EventKeyboard::KeyCode::KEY_A;
    note_to_string[35] = "A";

    auto audio = CocosDenshion::SimpleAudioEngine::getInstance();

    audio->playBackgroundMusic("/home/jonas/Downloads/Untitled.wav", true);

    create_tab_sprite();

    return true;
}

void HelloWorld::create_tab_sprite()
{
  const size_t num_notes = current_notes.size();

  note_sprites.reserve(num_notes);

  const int max_song_length_secs = 10*60;
  const int pixels_per_sec = 200;

  for (const Note& note : current_notes)
  {
    NoteSprite note_sprite;
    std::string text;

    auto label = Label::createWithTTF(note_to_string[note.note_id], "fonts/Marker Felt.ttf",32);

    label->setPosition(cocos2d::Point(note.start_time*pixels_per_sec , 50));

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
        player_health -= 10.0;
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
        std::cout << "HIT" << std::endl;
        hit = true;
      }
    }

    if (!hit)
    {
      player_health -= 0.1;
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
