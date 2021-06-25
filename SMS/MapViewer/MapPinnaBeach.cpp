#include "MapPinnaBeach.h"
#include "../../Common/CommonTypes.h"
#include "../../Memory/Memory.h"

#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QVBoxLayout>

using namespace memory;

MapPinnaBeach::MapPinnaBeach(QWidget* parent)
  : QGraphicsView(parent) {
  startTimer(16);

  initialize();
}

MapPinnaBeach::~MapPinnaBeach() {
}

void MapPinnaBeach::initialize() {
  scale(1 / 10.0, 1 / 10.0);

  scene_ = new QGraphicsScene(this);
  setScene(scene_);

  // マップ画像表示(ピンナビーチ)
  // (155, 768) -> (-5846.752, 5083.398) 青コイン4, (313, 1211) -> (-5007.95, 7431.883) 青コイン5
  const QPixmap pinna_beach_pix(":/sms/PinnaBeach.png");
  QGraphicsPixmapItem* pinna_beach_item = set_map_image(pinna_beach_pix, 155, 768, 313, -5846.752, 5083.398, -5007.95);

  // 水ヒットコイン
  coin_pix_ = QPixmap(":sms/coin.png");
  coin_blue_pix_ = QPixmap(":sms/coin_blue.png");
  fruit_banana_pix_ = QPixmap(":sms/fruit_banana.png");
  fruit_coconut_pix_ = QPixmap(":sms/fruit_coconut.png");
  fruit_durian_pix_ = QPixmap(":sms/fruit_durian.png");
  fruit_papaya_pix_ = QPixmap(":sms/fruit_papaya.png");
  fruit_pine_pix_ = QPixmap(":/sms/fruit_pine.png");
  for (s64 i = 0; i < 200; i++) {
    QPen pen(Qt::black);
    pen.setWidth(50);
    QBrush brush(Qt::yellow);
    //water_hit_obj_items_[i] = scene_->addPixmap(coin_pix_);
    //water_hit_obj_items_[i]->setTransformOriginPoint(coin_pix_.width() / 2.0, coin_pix_.height() / 2.0);
    water_hit_obj_items_[i] = scene_->addPixmap(QPixmap());
    water_hit_obj_items_[i]->setScale(200.0 / coin_pix_.width());
    water_hit_obj_items_[i]->setPos(0, 5000);
    water_hit_obj_items_[i]->setOpacity(0.5);
    water_hit_obj_items_[i]->setVisible(true);
  }

  // マリオの画像
  QPixmap mario_pix(":sms/mario.png");
  mario_pix_item_ = scene_->addPixmap(mario_pix);
  mario_pix_item_->setScale(200.0 / mario_pix.width());
  mario_pix_item_->setPos(0, 5000);
  mario_pix_item_->setTransformOriginPoint(mario_pix.width() / 2, mario_pix.height() / 2);
}

// 画像の座標とそれに対応するゲーム内の座標2つずつを用いてマップ画像をセットする
QGraphicsPixmapItem* MapPinnaBeach::set_map_image(const QPixmap& pixmap, double pix_x1, double pix_y1, double pix_x2, double game_x1, double game_y1, double game_x2) {
  const double scale = (game_x2 - game_x1) / (pix_x2 - pix_x1);
  QGraphicsPixmapItem* pixmap_item = scene_->addPixmap(pixmap);
  pixmap_item->setScale(scale);
  // p1->g1, p2->g2 >> g-g1 = scale*(p-p1)
  // 特にp=0のとき g0 = g1-scale*p1
  // p=0は画像の左上の座標なので、画像上のp=0にあたるゲーム内座標のg0に画像をセットすると丁度いい↓
  pixmap_item->setPos(QPointF(game_x1, game_y1) - scale * QPointF(pix_x1, pix_y1));
  //resize(QSize(3840, 2160) * scale / 10.0);
  return pixmap_item;
}

void MapPinnaBeach::timerEvent(QTimerEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  u32 p_mario = read_u32(0x8040a378);
  float mario_x = read_float(p_mario + 0x10);
  float mario_z = read_float(p_mario + 0x18);
  u16 mario_spin_angle = read_u16(p_mario + 0x9a);
  mario_pix_item_->setPos(mario_x, mario_z);
  mario_pix_item_->setRotation(-mario_spin_angle / 65536.0 * 360.0 - 180.0);

  const s8 current_stage = read_s8(0x805f8749);
  //const s8 current_episode = read_s8(0x805f874a);
  if (current_stage != 5) {
    return;
  }

  // Map Object Manager
  const u32 p_map_obj_manager = read_u32(0x8040a4d0);
  const u32 map_objects_count = read_u32(p_map_obj_manager + 0x14);
  const u32 p_map_objects = read_u32(p_map_obj_manager + 0x18);
  for (s64 i = 0; i < map_objects_count; i++) {
    const u32 p_object = read_u32(p_map_objects + 4 * i);
    if (!(0x80000000 <= p_object && p_object <= 0x817fffff))
      return;

    u16 state = read_u16(p_object + 0xfc);
    if (state == 0) {
      water_hit_obj_items_[i]->setVisible(false);
      water_hit_obj_items_[i]->setPos(0, 5000);
      continue;
    }

    water_hit_obj_items_[i]->setVisible(true);
    float x = read_float(p_object + 0x10);
    float z = read_float(p_object + 0x18);
    water_hit_obj_items_[i]->setPos(x, z);


    u32 function_pointer = read_u32(p_object);
    // 水ヒットコイン
    if (function_pointer == 0x803cf31c) {
      u32 spawn_obj_id = read_u32(p_object + 0x134);
      switch (spawn_obj_id) {
      case 100: // coin
        water_hit_obj_items_[i]->setPixmap(coin_pix_);
        water_hit_obj_items_[i]->setTransformOriginPoint(coin_pix_.width() / 2.0, coin_pix_.height() / 2.0);
        break;
      case 1000:  // banana
        water_hit_obj_items_[i]->setPixmap(fruit_banana_pix_);
        water_hit_obj_items_[i]->setTransformOriginPoint(fruit_banana_pix_.width() / 2.0, fruit_banana_pix_.height() / 2.0);
        break;
      case 1001:  // durian
        water_hit_obj_items_[i]->setPixmap(fruit_durian_pix_);
        water_hit_obj_items_[i]->setTransformOriginPoint(fruit_durian_pix_.width() / 2.0, fruit_durian_pix_.height() / 2.0);
        break;
      case 1002:  // papaya
        water_hit_obj_items_[i]->setPixmap(fruit_papaya_pix_);
        water_hit_obj_items_[i]->setTransformOriginPoint(fruit_papaya_pix_.width() / 2.0, fruit_papaya_pix_.height() / 2.0);
        break;
      case 1003:  // pine
        water_hit_obj_items_[i]->setPixmap(fruit_pine_pix_);
        water_hit_obj_items_[i]->setTransformOriginPoint(fruit_pine_pix_.width() / 2.0, fruit_pine_pix_.height() / 2.0);
        break;
      case 1004:  // coconut
        water_hit_obj_items_[i]->setPixmap(fruit_coconut_pix_);
        water_hit_obj_items_[i]->setTransformOriginPoint(fruit_coconut_pix_.width() / 2.0, fruit_coconut_pix_.height() / 2.0);
        break;
      default:
        water_hit_obj_items_[i]->setPixmap(QPixmap());
        water_hit_obj_items_[i]->setPos(0, 5000);
        break;
      }
    } else {
      water_hit_obj_items_[i]->setPos(0, 5000);
    }
  }

  //const u32 p_item_obj_manager = read_u32(0x8040a4d0);
  //const u32 map_objects_count = read_u32(p_map_obj_manager + 0x14);
  //const u32 p_map_objects = read_u32(p_map_obj_manager + 0x18);


  if (center_on_mario_)
    centerOn(mario_pix_item_);

  QGraphicsView::timerEvent(event);
}