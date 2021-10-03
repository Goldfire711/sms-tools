#include "MapPianta8.h"

#include "../../DolphinProcess/DolphinAccessor.h"
#include "../../Memory/Memory.h"

#include <iostream>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QtMath>
#include <QMouseEvent>

using namespace memory;

MapPianta8::MapPianta8(QWidget* parent, SMSData* sms_data) : QGraphicsView(parent), sms_data_(sms_data) {
  startTimer(16);

  setWindowTitle(tr("Map Viewer"));
  resize(1600, 1600);
  initialize();
  sms_data_->map_viewer_is_active = true;

  // ドラッグでスクロール操作
  setDragMode(ScrollHandDrag);
}

MapPianta8::~MapPianta8() {
  sms_data_->map_viewer_is_active = false;
}

void MapPianta8::initialize() {
  scale(1 / 25.0, 1 / 25.0);
  scene_ = new QGraphicsScene(this);
  QBrush red_brush(Qt::red);
  QPen white_pen(QColor(0xff, 0xff, 0xff, 0x80));
  white_pen.setWidth(100);
  QPen red_pen(Qt::red);
  red_pen.setWidth(50);

  // PV4(PV8)のマップを表示
  QPixmap PV4_pix(":/sms/PiantaVillageDay.png");
  QGraphicsPixmapItem* PV4_item = scene_->addPixmap(PV4_pix);
  const double PV4_item_scale = (19968.0 * 2.0) / PV4_pix.width();
  PV4_item->setScale(PV4_item_scale);
  PV4_item->setOffset(QPointF(-19968, -19968) / PV4_item_scale);
  scene_rect_ = QRectF(-19968, -19968, 19968 * 2, 19968 * 2);

  // 綿毛の生成範囲と綿毛の消失する範囲を描画
  scene_->addRect(-5000, -5000, 5000 * 2, 5000 * 2, white_pen);
  scene_->addRect(-5425, -5425, 5425 * 2, 5425 * 2, white_pen);
  scene_->addRect(-14848, -19968, 14848 * 2, 19968 * 2, white_pen);
  setScene(scene_);

  // 綿毛の固定生成位置を描画(-4225, 9139.115, -3020)
  auto fixed_fluff_radius = 500;
  QPen fixed_fluff_pen(QColor(0, 0, 0xff, 0xff));
  fixed_fluff_pen.setWidth(100);
  auto* fixed_fluff =
    scene_->addEllipse(-4225 - fixed_fluff_radius, -3020 - fixed_fluff_radius,
                       fixed_fluff_radius * 2, fixed_fluff_radius * 2, fixed_fluff_pen);

  // 綿毛x32初期化
  for (auto i = 0; i < 32; ++i) {
    // 綿毛の円の初期化
    QPen fluff_pen(Qt::black);
    fluff_pen.setWidth(50);
    QBrush fluff_brush(QColor(0xff, 0xff, 0xff, 0xff));
    fluff_[i] =
      scene_->addEllipse(0, 0, 0, 0, fluff_pen, fluff_brush);

    // 綿毛のレールの初期化
    QPen fluff_rail_pen(Qt::black);
    fluff_rail_pen.setWidth(50);
    fluff_rail_[i] = scene_->addLine(0, 0, 0, 0, fluff_rail_pen);

    // 綿毛idの初期化
    fluff_id_[i] = scene_->addText(QString::number(i));
    fluff_id_[i]->setScale(30);

    // 綿毛ワープの矢印(仮)
    QPen fluff_warp_line_pen(QColor(0xff, 0, 0, 0xff));
    fluff_warp_line_pen.setWidth(100);
    fluff_warp_line_[i] = scene_->addLine(0, 0, 10, 10, fluff_warp_line_pen);
    fluff_warp_line_[i]->setVisible(false);
  }

  // 青鳥レール
  QPolygonF blue_bird_rail;
  blue_bird_rail << QPointF(100, -1600) << QPointF(800, 1200) << QPointF(3000, 1500)
    << QPointF(4200, -400) << QPointF(2400, -2600);
  scene_->addPolygon(blue_bird_rail, white_pen);

  // 青鳥本体
  QPolygonF blue_bird_triangle;
  blue_bird_triangle << QPointF(0, 150) << QPointF(-250, -150) << QPointF(250, -150);
  blue_bird_ = scene_->addPolygon(blue_bird_triangle, Qt::NoPen, Qt::blue);

  // 青鳥前/次ノード
  blue_bird_next_node_ = scene_->addEllipse(-150, -150, 300, 300, QPen(Qt::black), Qt::blue);
  blue_bird_previous_node_ =
    scene_->addEllipse(-150, -150, 300, 300, QPen(Qt::black), QBrush(Qt::NoBrush));

  // 赤コイン・青コインの画像 (gpItemManager)
  coin_red_pix_ = QPixmap(":/sms/coin_red.png");
  coin_blue_pix_ = QPixmap(":/sms/coin_blue.png");
  for (s64 i = 0; i < 85; ++i) {
    coins_pix_item_[i] = scene_->addPixmap(coin_red_pix_);
    coins_pix_item_[i]->setScale(200.0 * 2 / 35.0);
    coins_pix_item_[i]->setTransformOriginPoint(coin_red_pix_.width() / 2,
                                                coin_red_pix_.height() / 2);
    coins_pix_item_[i]->setVisible(false);
  }

  // シャインの画像
  QPixmap shine_pix(":/sms/shine.png");
  shine_pix_item_ = scene_->addPixmap(shine_pix);
  shine_pix_item_->setScale(200.0 * 2 / 35.0);
  shine_pix_item_->setTransformOriginPoint(shine_pix.width() / 2, shine_pix.height() / 2);
  shine_pix_item_->setVisible(false);

  // マリオの画像
  QPixmap mario_pix(":/sms/mario.png");
  mario_pix_item_ = scene_->addPixmap(mario_pix);
  mario_pix_item_->setScale(200.0 * 2 / 35.0);
  mario_pix_item_->setTransformOriginPoint(mario_pix.width() / 2, mario_pix.height() / 2);

  // 綿毛のターゲットとその範囲の描画初期化
  const auto target_radius = 50.0;
  target_ = scene_->addEllipse(0, 0, target_radius * 2, target_radius * 2, Qt::NoPen, Qt::red);
  range_ = scene_->addRect(0, 0, 0, 0, red_pen);
}

void MapPianta8::timerEvent(QTimerEvent* event) {
  scene_->setSceneRect(scene_rect_);

  QPen redPen(Qt::red);
  redPen.setWidth(50);
  QBrush redBrush(Qt::red);
  // 綿毛のターゲットとその範囲の描画
  const auto radius = 50.0;
  target_->setPos(sms_data_->m_target.x - radius, sms_data_->m_target.z - radius);
  range_->setRect(sms_data_->m_range.min.x, sms_data_->m_range.min.z,
                  sms_data_->m_range.max.x - sms_data_->m_range.min.x,
                  sms_data_->m_range.max.z - sms_data_->m_range.min.z);

  if (DolphinComm::DolphinAccessor::getStatus() ==
    DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
    // ステージ判定 綿毛ステージ(8-7)じゃなければreturn)
    const s8 current_stage = read_s8(0x805F8749);
    const s8 current_episode = read_s8(0x805F874A);
    if (current_stage != 8 || current_episode != 7) {
      for (s64 i = 0; i < 32; ++i) {
        former_state_[i] = 2;
      }
      return;
    }
    // 綿毛x32の描画
    const u32 fluff_base_table = read_u32(0x811b0c44);
    for (auto i = 0; i < 32; ++i) {
      const u32 fluff_base = read_u32(fluff_base_table + i * 4);

      const float x = read_float(fluff_base + 0x10);
      const float z = read_float(fluff_base + 0x18);
      const float x_offset = read_float(fluff_base + 0x154);
      const float z_offset = read_float(fluff_base + 0x15c);
      const float x_origin = read_float(fluff_base + 0x10c);
      const float z_origin = read_float(fluff_base + 0x114);
      const float amplitude = read_float(fluff_base + 0x138);
      const float sine = read_float(fluff_base + 0x140);
      const float cosine = read_float(fluff_base + 0x144);
      const s16 state = read_s16(fluff_base + 0xfc);

      const float x_edge1 = x_origin + x_offset + amplitude * (-1) * (cosine + sine);
      const float z_edge1 = z_origin + z_offset + amplitude * (-1) * (sine - cosine);
      const float x_edge2 = x_origin + x_offset + amplitude * (cosine + sine);
      const float z_edge2 = z_origin + z_offset + amplitude * (sine - cosine);

      const float y = read_float(fluff_base + 0x14);

      // 透明度0(通常通り)
      // 綿毛の円の位置と色の設定
      const float fluff_radius = 200 + y / 9139 * 300;
      fluff_[i]->setRect(x - fluff_radius, z - fluff_radius, fluff_radius * 2, fluff_radius * 2);
      switch (state) {
      case 0: // 次に固定湧きポイントにワープする綿毛(青)
        fluff_[i]->setBrush(QColor(0, 0, 0xff, 0xff));
        if (former_state_[i] != 0 && former_state_[i] != 2) {
          fluff_warp_line_[i]->setLine(former_x_[i], former_z_[i], x, z);
          fluff_warp_line_[i]->setVisible(true);
          QTimer::singleShot(1000, this, [this, i]() {
            fluff_warp_line_[i]->setVisible(false);
          });
        }
        former_state_[i] = state;
        former_x_[i] = x;
        former_z_[i] = z;
        break;
      case 1: // 通常の綿毛(白)
        fluff_[i]->setBrush(QColor(0xff, 0xff, 0xff, 0xff));
        break;
      case 2: // ワープ直後の綿毛(紫)
        fluff_[i]->setBrush(QColor(0xff, 0, 0xff, 0xff));
        if (former_state_[i] != 2) {
          fluff_warp_line_[i]->setLine(former_x_[i], former_z_[i], x, z);
          fluff_warp_line_[i]->setVisible(true);
          QTimer::singleShot(1000, this, [this, i]() {
            fluff_warp_line_[i]->setVisible(false);
          });
        }
        break;
      case 3: // 壁にぶつかった直後(赤)
        fluff_[i]->setBrush(QColor(0xff, 0, 0, 0xff));
        break;
      case 4: // ワープ直前の綿毛(黒)
        fluff_[i]->setBrush(QColor(0, 0, 0, 0xff));
        former_state_[i] = state;
        former_x_[i] = x;
        former_z_[i] = z;
        break;
      default:
        break;
      }

      // 綿毛のレール
      fluff_rail_[i]->setLine(x_edge1, z_edge1, x_edge2, z_edge2);

      // 綿毛id
      fluff_id_[i]->setPos(x, z);
    }

    // 青鳥描画
    const u32 blue_bird_base = read_u32(0x80f9741c);
    const float blue_bird_x = read_float(blue_bird_base + 0x10);
    const float blue_bird_z = read_float(blue_bird_base + 0x18);
    const float blue_bird_degree = read_float(blue_bird_base + 0x34);

    blue_bird_->setPos(blue_bird_x, blue_bird_z);
    blue_bird_->setRotation(-blue_bird_degree);

    // 青鳥前ノード描画
    const s32 blue_bird_next_node = read_s32(blue_bird_base + 0x218);
    switch (blue_bird_next_node) {
    case 0:
      blue_bird_next_node_->setPos(100, -1600);
      break;
    case 1:
      blue_bird_next_node_->setPos(800, 1200);
      break;
    case 2:
      blue_bird_next_node_->setPos(3000, 1500);
      break;
    case 3:
      blue_bird_next_node_->setPos(4200, -400);
      break;
    case 4:
      blue_bird_next_node_->setPos(2400, -2600);
      break;
    }

    // 青鳥次ノード描画
    const s32 blue_bird_previous_node = read_s32(blue_bird_base + 0x21c);
    switch (blue_bird_previous_node) {
    case 0:
      blue_bird_previous_node_->setPos(100, -1600);
      break;
    case 1:
      blue_bird_previous_node_->setPos(800, 1200);
      break;
    case 2:
      blue_bird_previous_node_->setPos(3000, 1500);
      break;
    case 3:
      blue_bird_previous_node_->setPos(4200, -400);
      break;
    case 4:
      blue_bird_previous_node_->setPos(2400, -2600);
      break;
    }

    // 赤・青コイン描画
    const u32 item_manager_base = read_u32(0x8040a4d8);
    const u32 item_base_table = read_u32(item_manager_base + 0x18);
    for (s64 i = 0; i < 85; ++i) {
      const u32 base = read_u32(item_base_table + i * 4);
      const u32 vtable = read_u32(base);

      if (vtable == 0x803C15AC) // 赤コイン
      {
        const s16 state = read_s16(base + 0xfc);

        if (state != 0) {
          const float x = read_float(base + 0x10);
          const float z = read_float(base + 0x18);

          // 赤コイン描画
          coins_pix_item_[i]->setPos(x, z);
          coins_pix_item_[i]->setScale(400 * 2 / 35.0);

          coins_pix_item_[i]->setPixmap(coin_red_pix_);
          coins_pix_item_[i]->setVisible(true);
        } else {
          coins_pix_item_[i]->setVisible(false);
        }
      } else if (vtable == 0x803C13C8) // 青コイン
      {
        const s16 state = read_s16(base + 0xfc);

        if (state != 0) {
          const float x = read_float(base + 0x10);
          const float z = read_float(base + 0x18);

          // 青コイン描画
          coins_pix_item_[i]->setPos(x, z);
          coins_pix_item_[i]->setScale(400 * 2 / 35.0);

          coins_pix_item_[i]->setPixmap(coin_blue_pix_);
          coins_pix_item_[i]->setVisible(true);
        } else {
          coins_pix_item_[i]->setVisible(false);
        }
      } else {
        coins_pix_item_[i]->setVisible(false);
      }

    }

    // shineを描画
    const u32 shine_base_pianta_8 = read_u32(0x80f87fec, {0x18, 0});
    const s16 shine_state = read_s16(shine_base_pianta_8 + 0xfc);
    if (shine_state != 0) {
      const float shine_x = read_float(shine_base_pianta_8 + 0x10);
      const float shine_z = read_float(shine_base_pianta_8 + 0x18);

      shine_pix_item_->setPos(shine_x, shine_z);
      shine_pix_item_->setScale(400 * 2 / 35.0);
      shine_pix_item_->setVisible(true);
    } else {
      shine_pix_item_->setVisible(false);
    }

    // marioを描画
    const u32 mario_base = read_u32(0x8040a378);
    const float mario_x = read_float(mario_base + 0x10);
    const float mario_y = read_float(mario_base + 0x14);
    const float mario_z = read_float(mario_base + 0x18);
    const u16 mario_angle = read_float(mario_base + 0x98);

    float mario_radius = 500;
    if (mario_y < 0) {
      mario_radius = 200;
    } else if (mario_y <= 9139) {
      mario_radius = 200 + mario_y / 9139 * 300;
    }
    mario_pix_item_->setPos(mario_x, mario_z);
    mario_pix_item_->setScale(mario_radius * 2 / 35.0);
    mario_pix_item_->setRotation(-mario_angle / 65536.0 * 360.0 - 180.0);

    //centerOn(mario_x, mario_z);
  }
  QGraphicsView::timerEvent(event);
}

void MapPianta8::wheelEvent(QWheelEvent* event) {
  qreal view_scale = 1;
  if (event->angleDelta().y() > 0) {
    view_scale = 1.1;
  }
  else if (event->angleDelta().y() < 0) {
    view_scale = 1 / 1.1;
  }
  scale(view_scale, view_scale);
}
