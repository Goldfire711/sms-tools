#include "Items.h"

void ItemMap::set_map() {
  maps_.clear();

  constexpr u32 p_director = 0x80907a20;
  stage_ = read_s8(p_director + 0x7c);
  episode_ = read_s8(p_director + 0x7d);

  // TODO
  switch (stage_) {
  case SIRENA:
    maps_.append({ QPixmap(":stages/SirenaBeach_x10.png") , INFINITY });
    setScale(10.0);
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
      setScale(20.0);
      break;
    }
    break;
  }

  if (maps_.isEmpty())
    return;
  setPixmap(maps_[0].pix);
  setTransformOriginPoint(maps_[0].pix.width() / 2.0, maps_[0].pix.height() / 2.0);
  setTransform(QTransform().translate(-maps_[0].pix.width() / 2.0, -maps_[0].pix.height() / 2.0));
}

void ItemMap::update(const float mario_y) {
  if (maps_.count() <= 1)
    return;
  
  s64 id = 0;
  for (s64 i = 0; i < maps_.count(); i++) {
    if (mario_y <= maps_[i].height)
      id = i;
  }
  if (id != former_id_) {
    setPixmap(maps_[id].pix);
    former_id_ = id;
  }
}

bool ItemMap::is_changed() const {
  constexpr u32 p_director = 0x80907a20;
  const s8 current_stage = read_s8(p_director + 0x7c);
  const s8 current_episode = read_s8(p_director + 0x7d);
  
  return (stage_ != current_stage) || (episode_ != current_episode);
}