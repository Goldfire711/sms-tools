#include "MapSirena1.h"
#include "../../Memory/Memory.h"

#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDebug>
#include <QtMath>

using namespace memory;



MapSirena1::MapSirena1(QWidget *parent)
  : QGraphicsView(parent) {
  startTimer(16);
  initialize();
}

MapSirena1::~MapSirena1() {}

void MapSirena1::initialize() {
  scale(0.1, 0.1);

  auto* scene = new QGraphicsScene(this);
  setScene(scene);

  const QPixmap pix_sirena_beach(":/sms/SirenaBeach_19200,10800.png");
  auto* item_sirena_beach = scene->addPixmap(pix_sirena_beach);
  item_sirena_beach->setTransformOriginPoint(pix_sirena_beach.width() / 2.0, pix_sirena_beach.height() / 2.0);
  item_sirena_beach->setTransform(QTransform().translate(-pix_sirena_beach.width() / 2.0, -pix_sirena_beach.height() / 2.0));
  item_sirena_beach->setScale(10.0);

  // マリオ
  QPixmap pix_mario(":sms/mario.png");
  mario_ = scene->addPixmap(pix_mario);
  mario_->setTransformOriginPoint(pix_mario.width() / 2.0, pix_mario.height() / 2.0);
  mario_->setTransform(QTransform().translate(-pix_mario.width() / 2.0, -pix_mario.height() / 2.0));
  mario_->setScale(200.0 / pix_mario.width());

  // マンタ
  pix_manta_ = QPixmap(":sms/manta_red.png");
  pix_manta_purple_ = QPixmap(":sms/manta_purple.png");
  for (s64 i = 0; i < 128; i++) {
    manta_[i] = scene->addPixmap(pix_manta_);
    manta_[i]->setTransformOriginPoint(pix_manta_.width() / 2.0, pix_manta_.height() / 2.0);
    manta_[i]->setTransform(QTransform().translate(-pix_manta_.width() / 2.0, -pix_manta_.height() / 2.0));
    manta_[i]->setVisible(false);
  }
  for (s64 i = 0; i < 128; i++) {
    QPen pen1(Qt::blue);
    pen1.setWidth(50);
    manta_line1_[i] = scene->addLine(0, 0, 0, 0, pen1);
    manta_line1_[i]->setVisible(false);

    QPen pen2(Qt::red);
    pen2.setWidth(50);
    manta_line2_[i] = scene->addLine(0, 0, 0, 0, pen2);
    manta_line2_[i]->setVisible(false);
  }
  for (s64 i = 0; i < 128; i++) {
    manta_id_[i] = scene->addText(QString::number(i));
    manta_id_[i]->setScale(10);
    manta_id_[i]->setDefaultTextColor(Qt::white);
    manta_id_[i]->setVisible(false);
  }

  setDragMode(ScrollHandDrag);
  setSceneRect(-19200, -10800, 38400, 21600);
  
}

void MapSirena1::timerEvent(QTimerEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  // マリオ
  const u32 p_mario = read_u32(0x8040a378);
  const float mario_x = read_float(p_mario + 0x10);
  const float mario_z = read_float(p_mario + 0x18);
  u16 mario_spin_angle = read_u16(p_mario + 0x9a);
  mario_->setPos(mario_x, mario_z);
  mario_->setRotation(-mario_spin_angle / 65536.0 * 360.0 - 180.0);

  // ステージ・エピソード確認
  const s8 current_stage = read_s8(0x805f8749);
  const s8 current_episode = read_s8(0x805f874a);
  if (current_stage != 6 || current_episode != 0)
    return;

  // マンタ
  constexpr u32 p_manta_manager = 0x81056e50;
  const u32 p_mantas = read_u32(p_manta_manager + 0x18);
  for (s64 i = 0; i < 128; i++) {
    const u32 p = read_u32(p_mantas + 4 * i);
    const u32 draw_info = read_u32(p + 0xf0);
    const bool is_visible = !(draw_info & 1);
    manta_[i]->setVisible(is_visible);
    //manta_line1_[i]->setVisible(is_visible && !is_zigzag && show_lines_);
    //manta_line2_[i]->setVisible(is_visible && is_zigzag && show_lines_);
    manta_line1_[i]->setVisible(is_visible && show_lines_);
    manta_line2_[i]->setVisible(is_visible && show_lines_);
    manta_id_[i]->setVisible(is_visible);
    if (!is_visible) {
      continue;
    }

    const float scale = read_float(p + 0x24);
    const float x = read_float(p + 0x10);
    const float z = read_float(p + 0x18);
    const float target_x = read_float(p + 0x158);
    const float target_z = read_float(p + 0x160);
    const float target2_x = read_float(p + 0x164);
    const float target2_z = read_float(p + 0x16c);
    const s32 is_zigzag = read_s32(p + 0x1a4);
    const float cos = read_float(p + 0x170);
    const float sin = read_float(p + 0x178);
    float rad = acos(cos);
    if (sin < 0)
      rad *= -1;
    const s32 form = read_s32(p + 0x18c);
    constexpr s32 hps[] = { 16, 8, 4, 2, 1, 1 };
    const s32 hp = hps[form];
    const s32 damage = read_s32(p + 0x19c);
    const s32 cool_time = read_s32(p + 0x1a0);
    const u32 p_anm = read_u32(p + 0x74, { 0xc });
    const s16 anm_end_frame = read_s16(p_anm + 0xc);
    const float anm_speed = read_float(p_anm + 0x10);
    const float cur_anm_frame = read_float(p_anm + 0x14);
    const u32 p_ai = read_u32(p + 0x8c);
    const u32 cur_nerve = read_u32(p_ai + 0x14);

    manta_[i]->setScale(scale);
    manta_[i]->setPos(x, z);
    manta_[i]->setRotation(180 * rad / M_PI - 90);
    if (cur_nerve == 0x8040B0F4) {
      manta_[i]->setPixmap(pix_manta_purple_);
    } else {
      manta_[i]->setPixmap(pix_manta_);
    }

    manta_line1_[i]->setLine(x, z, target_x, target_z);
    manta_line2_[i]->setLine(x, z, target2_x, target2_z);

    QString text = QString();
    if (show_id_)
      text += QString::number(i) + "\n";
    if (show_dmg_)
      text += QString::number(damage) + "/" + QString::number(hp) + "\n";
    if (show_ct_)
      text += QString::number(cool_time) + "\n";
    if (show_anm_)
      text += QString::number(cur_anm_frame, 'f', 1) + "/" + QString::number(anm_end_frame) + "\n";
    if (show_anm_spd_)
      text += QString::number(anm_speed, 'f', 1) + "/f";
    manta_id_[i]->setPlainText(text);
    manta_id_[i]->setPos(x, z);
  }

  if (center_on_mario_)
    centerOn(mario_);

  QGraphicsView::timerEvent(event);
}

void MapSirena1::wheelEvent(QWheelEvent* event) {
  qreal view_scale = 1;
  if (event->angleDelta().y() > 0) {
    view_scale = 1.1;
  } else if (event->angleDelta().y() < 0) {
    view_scale = 1 / 1.1;
  }
  scale(view_scale, view_scale);
}