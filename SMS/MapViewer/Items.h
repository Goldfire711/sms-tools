#pragma once

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include "../../Common/CommonTypes.h"
#include "../../Memory/Memory.h"
#include <json.hpp>
#include <sstream>

using namespace memory;
using json = nlohmann::json;
extern json g_vtable_to_class;
extern json g_class_to_png;

class ItemMap : public QGraphicsPixmapItem {

public:
  void set_map();
  void update(const float mario_y);
  bool is_changed() const;

  s8 stage_ = 0;
  s8 episode_ = 0;
private:
  struct map_height {
    QPixmap pix;
    float height;
  };
  QVector<map_height> maps_;
  s64 former_id_ = 0;

  enum {
    AIRPORT1 = 0,
    DOLPIC = 1,
    BIANCO = 2,
    RICO = 3,
    MAMMA = 4,
    PINNA_BEACH = 5,
    SIRENA = 6,
    SIRENA_DELFINO = 7,
    MONTE = 8,
    MARE = 9,
    PINNA_PARCO = 13,
    SIRENA_CASINO = 14,
    OPTION = 15,
    MARE_EX_EP8 = 16,
    AIRPORT2 = 20,
    DOLPIC_EX_SLIDE = 21,
    DOLPIC_EX_PACHINKO = 22,
    DOLPIC_EX_GRASS = 23,
    DOLPIC_EX_LILYPAD = 24,
    DOLPIC_EX_TURBO = 29,
    RICO_EX_EP2 = 30,
    MARE_EX_EP6 = 31,
    MAMMA_EX_EP1 = 32,
    MAMMA_EX_EP4 = 33,
    SIRENA_EX_EP4 = 40,
    PINNA_EX_EP6 = 41,
    MONTE_EX_EP5 = 42,
    MARE_EX_EP3 = 44,
    BIANCO_EX_EP6 = 46,
    BIANCO_EX_EP3 = 47,
    RICO_EX_EP4 = 48,
    PINNA_EX_EP2 = 50,
    SIRENA_EX_EP2 = 51,
    CORONA = 52,
    BIANCO_BOSS_EP2 = 55,
    SIRENA_BOSS_EP5 = 56,
    MARE_BOSS_EP4 = 57,
    PINNA_BOSS = 58,
    RICO_BOSS_EP1 = 59,
    CORONA_BOSS = 60,
  };
};

class ItemMario : public QGraphicsPixmapItem {
public:
  ItemMario();
  void update();

  u32 p_mario_ = 0;
  float x_ = 0;
  float y_ = 0;
  float z_ = 0;
  u16 spin_angle_ = 0;
};

class ItemObjBase : public QGraphicsPixmapItem {
public:
  ItemObjBase(const u32 p_obj, const std::string class_name);
  void update();

  u32 p_obj_ = 0;
  u32 vt_ = 0;
  float x_ = 0;
  float y_ = 0;
  float z_ = 0;
  float y_rot_ = 0;
  float scale_ = 1;
  u32 draw_info_ = 0;

  std::string class_name_;
};

class ItemGroupManagerBase : public QGraphicsItemGroup {
public:
  ItemGroupManagerBase(const u32 p_manager);
  ~ItemGroupManagerBase();
  virtual void update();

  u32 p_manager_ = 0;
  std::string class_name_;
  QVector<ItemObjBase*> objs_;
};