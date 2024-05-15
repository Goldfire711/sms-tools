#include "Items.h"

void ItemMap::set_map() {
  maps_.clear();
  selected_map_layer_ = -1;

  constexpr u32 p_director = 0x80907a20;
  stage_ = read_s8(p_director + 0x7c);
  episode_ = read_s8(p_director + 0x7d);

  // TODO
  switch (stage_) {
  case SIRENA:
    maps_.append({ QPixmap(":stages/SirenaBeach_x10.png") , INFINITY });
    setScale(10);
    break;
  case MONTE:
    switch (episode_) {
    case 0:
    case 2:
    case 4:
    case 6:
      maps_.append({ QPixmap(":stages/PiantaVillageNight_x20.png"), INFINITY });
      maps_.append({ QPixmap(":stages/PiantaVillageNight_x20_h7100.png"), 7100.0 });
      maps_.append({ QPixmap(":stages/PiantaVillageNight_x20_h-800.png"), -800.0 });
      setScale(20);
      break;
    default:
      maps_.append({ QPixmap(), INFINITY });
    }
    break;
  default:
    maps_.append({ QPixmap(), INFINITY });
    break;
  }
  maps_.append({ QPixmap(), -INFINITY });

  cur_map_ = &maps_.front();
  current_map_height_max_ = maps_[0].height;
  current_map_height_min_ = maps_[1].height;
  former_map_ = nullptr;
  setPixmap(cur_map_->pix);
  setTransformOriginPoint(boundingRect().center());
  setTransform(QTransform().translate(-boundingRect().width() / 2, -boundingRect().height() / 2));
}

void ItemMap::update() {
  if (maps_.count() <= 2)
    return;

  s32 cur_layer = 0;
  if (selected_map_layer_ == -1) {
    // Automatically select map layer by mario_y
    const u32 p_mario = read_u32(0x8040a378);
    const float mario_y = read_float(p_mario + 0x14);

    cur_map_ = &maps_.front();
    for (s32 i = 0; i < maps_.count() - 1; i++) {
      if (mario_y <= maps_[i].height) {
        cur_map_ = &maps_[i];
        cur_layer = i;
      }
    }
  } else {
    cur_layer = selected_map_layer_;
    cur_map_ = &maps_[cur_layer];
  }

  if (cur_map_ != former_map_) {
    setPixmap(cur_map_->pix);
    current_map_height_max_ = maps_[cur_layer].height;
    current_map_height_min_ = maps_[cur_layer + 1].height;
    former_map_ = cur_map_;
  }
}

bool ItemMap::is_changed() const {
  constexpr u32 p_director = 0x80907a20;
  const s8 current_stage = read_s8(p_director + 0x7c);
  const s8 current_episode = read_s8(p_director + 0x7d);
  
  return (stage_ != current_stage) || (episode_ != current_episode);
}
