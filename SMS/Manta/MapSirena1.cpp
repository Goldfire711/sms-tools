#include "MapSirena1.h"
#include "../../Memory/Memory.h"

#include <QGraphicsPixmapItem>
#include <QMouseEvent>
#include <QDebug>
#include <QtMath>

using namespace memory;

MapSirena1::MapSirena1(QWidget *parent)
  : QGraphicsView(parent) {
  timer_id_ = startTimer(100);
  initialize();
  refresh();
}

MapSirena1::~MapSirena1() {}

void MapSirena1::initialize() {
  scale(0.1, 0.1);

  auto* scene = new QGraphicsScene(this);
  setScene(scene);

  const QPixmap pix_sirena_beach(":stages/SirenaBeach_x10.png");
  auto* item_sirena_beach = scene->addPixmap(pix_sirena_beach);
  item_sirena_beach->setTransformOriginPoint(pix_sirena_beach.width() / 2.0, pix_sirena_beach.height() / 2.0);
  item_sirena_beach->setTransform(QTransform().translate(-pix_sirena_beach.width() / 2.0, -pix_sirena_beach.height() / 2.0));
  item_sirena_beach->setScale(10.0);

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
    QPen pen2(Qt::red);
    pen2.setWidth(20);
    manta_attractor_line_[i] = scene->addLine(0, 0, 0, 0, pen2);
    manta_attractor_line_[i]->setVisible(false);

    QPen pen1(Qt::blue);
    pen1.setWidth(20);
    manta_target_line_[i] = scene->addLine(0, 0, 0, 0, pen1);
    manta_target_line_[i]->setVisible(false);
  }
  for (s64 i = 0; i < 128; i++) {
    manta_info_[i] = scene->addText(QString::number(i));
    manta_info_[i]->setScale(10);
    manta_info_[i]->setDefaultTextColor(Qt::white);
    manta_info_[i]->setVisible(false);
  }

  // manta nodes
  // graphTracer(0x124)->graphWeb(0x0)->graphNodes[i](0x0 + i*0x10)->railNode(0x0)
  manta_nodes_ = { {514, 10726}, {-3144, 10499}, {-6615, 9746}, {-7161, 1339}, {-4783, -4575}, {-2654, -6743},
    {2779, -7322}, {5411, -6814}, {6888, -4280}, {6645, 2001}, {6020, 9348}, {3715, 10467}, {1000, 10734} };
  manta_nodes_poly_ = scene->addPolygon(QPolygon(manta_nodes_));
  pen_gray_ = QPen(Qt::gray);
  pen_gray_.setWidth(20);
  manta_nodes_poly_->setPen(pen_gray_);
  for (s64 i = 0; i < manta_nodes_.size(); i++) {
    manta_nodes_poly_id_[i] = scene->addText(QString::number(i));
    manta_nodes_poly_id_[i]->setDefaultTextColor(Qt::white);
    manta_nodes_poly_id_[i]->setScale(20);
    manta_nodes_poly_id_[i]->setPos(manta_nodes_[i]);
    manta_nodes_poly_id_[i]->setTransformOriginPoint(10, 10);
  }
  manta_targets1_ = scene->addPolygon(QPolygon(13));
  manta_targets1_->setPen(pen_gray_);
  for (s64 i = 0; i < 13; i++) {
    manta_target_id1_[i] = scene->addText(QString::number(i));
    manta_target_id1_[i]->setScale(20);
    manta_target_id1_[i]->setTransformOriginPoint(10, 10);
    manta_target_id1_[i]->setDefaultTextColor(Qt::white);
  }
  manta_targets2_ = scene->addPolygon(QPolygon(13));
  manta_targets2_->setPen(pen_gray_);
  for (s64 i = 0; i < 13; i++) {
    manta_target_id2_[i] = scene->addText(QString::number(i));
    manta_target_id2_[i]->setScale(20);
    manta_target_id2_[i]->setTransformOriginPoint(10, 10);
    manta_target_id2_[i]->setDefaultTextColor(Qt::white);
  }

  // 半径6000の円
  circle6000_ = scene->addEllipse(0, 0, 12000, 12000);
  pen_red_ = QPen(Qt::red);
  pen_red_.setWidth(20);

  // マリオ
  QPixmap pix_mario(":sms/mario.png");
  mario_ = scene->addPixmap(pix_mario);
  mario_->setTransformOriginPoint(pix_mario.width() / 2.0, pix_mario.height() / 2.0);
  mario_->setTransform(QTransform().translate(-pix_mario.width() / 2.0, -pix_mario.height() / 2.0));
  mario_->setScale(200.0 / pix_mario.width());

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
  if (p_manta_manager_ == 0)
    return;

  const u32 p_mantas = read_u32(p_manta_manager_ + 0x18);
  for (s64 i = 0; i < 128; i++) {
    const u32 p = read_u32(p_mantas + 4 * i);
    const u32 draw_info = read_u32(p + 0xf0);
    const bool is_visible = !(draw_info & 1);
    manta_[i]->setVisible(is_visible);
    manta_target_line_[i]->setVisible(is_visible && show_line_target_);
    manta_attractor_line_[i]->setVisible(is_visible && show_line_attractor_);
    manta_info_[i]->setVisible(is_visible);
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
    //const s32 is_targeting_mario = read_s32(p + 0x1a4);
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
    const s32 is_targeting_mario = read_s32(p + 0x1a4);
    const s32 timer = read_s32(p_ai + 0x20);
    const s32 escape_time = read_s32(p + 0x188);

    manta_[i]->setScale(scale);
    manta_[i]->setPos(x, z);
    manta_[i]->setRotation(180 * rad / M_PI - 90);
    if (cur_nerve == 0x8040B0F4) {
      manta_[i]->setPixmap(pix_manta_purple_);
    } else {
      manta_[i]->setPixmap(pix_manta_);
    }

    manta_target_line_[i]->setLine(x, z, target_x, target_z);
    const float target2_invsq = 1 / sqrt(target2_x * target2_x + target2_z * target2_z);
    manta_attractor_line_[i]->setLine(x, z, x + 300 * target2_x * target2_invsq, z + 300 * target2_z * target2_invsq);

    QString text = QString();
    if (show_id_)
      text += QString::number(i) + "\n";
    if (show_dmg_)
      text += QString::number(damage) + "/" + QString::number(hp) + "\n";
    if (show_ct_)
      text += QString::number(cool_time) + "\n";
    if (show_anm_)
      text += QString::number(cur_anm_frame, 'f', 1) + "/" + QString::number(anm_end_frame, 'f', 1) + "\n";
    if (show_anm_spd_)
      text += QString::number(anm_speed, 'f', 1) + "/f\n";
    if (show_is_chasing_)
      text += QString::number(is_targeting_mario) + ": " + QString::number(timer % escape_time) + "/" + QString::number(escape_time);
    manta_info_[i]->setPlainText(text);
    manta_info_[i]->setPos(x, z);
  }

  // 影に入ったとき・形態4のときにマンタは6000離れる
  if (show_circle_) {
    circle6000_->setVisible(true);
    circle6000_->setPos(mario_x - 6000, mario_z - 6000);
    const s8 sEscapeFromMario = read_s8(0x8040b120);
    if (sEscapeFromMario) {
      circle6000_->setPen(pen_red_);
    } else {
      circle6000_->setPen(pen_gray_);
    }
  } else {
    circle6000_->setVisible(false);
  }

  // ターゲット候補のnodes
  if (show_target_nodes_) {
    manta_nodes_poly_->setVisible(true);
    for (s64 i = 0; i < 13; i++)
      manta_nodes_poly_id_[i]->setVisible(true);
    // *0.6
    QVector<QPoint> targets1(13);
    for (s64 i = 0; i < 13; i++) {
      targets1[i] = (manta_nodes_[i] + 0.6 * (QPoint(mario_x, mario_z) - manta_nodes_[i]));
      manta_target_id1_[i]->setPos(targets1[i]);
      manta_target_id1_[i]->setVisible(true);
    }
    manta_targets1_->setPolygon(QPolygon(targets1));
    manta_targets1_->setVisible(true);
    // *0.8
    QVector<QPoint> targets2(13);
    for (s64 i = 0; i < 13; i++) {
      targets2[i] = (manta_nodes_[i] + 0.8 * (QPoint(mario_x, mario_z) - manta_nodes_[i]));
      manta_target_id2_[i]->setPos(targets2[i]);
      manta_target_id2_[i]->setVisible(true);
    }
    manta_targets2_->setPolygon(QPolygon(targets2));
    manta_targets2_->setVisible(true);
  } else {
    manta_nodes_poly_->setVisible(false);
    for (s64 i = 0; i < 13; i++) {
      manta_nodes_poly_id_[i]->setVisible(false);
      manta_target_id1_[i]->setVisible(false);
      manta_target_id2_[i]->setVisible(false);
    }
    manta_targets1_->setVisible(false);
    manta_targets2_->setVisible(false);
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

void MapSirena1::refresh() {
  const u32 addr = read_u32(0x8040a6e8);
  const s32 count = read_u32(addr + 0x14);
  const u32 head = read_u32(addr + 0x18);
  u32 l = head;
  for (s64 i = 0; i < count - 1; i++) {
    l = read_u32(l);
    u32 m = read_u32(l + 0x8);
    u32 vt = read_u32(m);
    if (vt == 0x803e177c) {
      p_manta_manager_ = m;
      return;
    }
  }
  p_manta_manager_ = 0;
  return;
}

void MapSirena1::set_timer_interval(s32 interval) {
  killTimer(timer_id_);
  timer_id_ = startTimer(interval);
}