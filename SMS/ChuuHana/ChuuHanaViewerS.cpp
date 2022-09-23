#include "ChuuHanaViewerS.h"

#include <QDebug>
#include <QtMath>
#include <Memory/Memory.h>
#include <QPainter>
#include <QMatrix4x3>
#include <QVBoxLayout>

using namespace memory;

ChuuHanaViewerS::ChuuHanaViewerS(QWidget* parent)
  : QWidget(parent) {
  elapsed_timer_.start();
}

ChuuHanaViewerS::~ChuuHanaViewerS() {
}

void ChuuHanaViewerS::paintEvent(QPaintEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  const s8 current_stage = read_s8(0x805f8749);
  const s8 current_episode = read_s8(0x805f874a);
  if (current_stage != 4 || current_episode != 1)
    return;

  // MirrorSの情報
  constexpr u32 p_mirror = 0x81341c04;
  constexpr u32 p_matrix = 0x81342a98;
  constexpr double radius = 1000.0;
  constexpr double mirror_Y = 333.3402099609375;
  constexpr u32 chuuhana_ids[] = { 0 };
  constexpr double willfall_r = 650.0;
  QPoint chuuhana_rail[] = {
    QPoint(-1600, -1000),
    QPoint(-1400, -600),
    QPoint(-1100, -500),
    QPoint(-600, -500),
    QPoint(-400, -1000),
    QPoint(-600, -1300),
    QPoint(-1000, -1500),
    QPoint(-1424, -1350)
  };

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  const int side = qMin(width(), height());
  double scale = side / 200.0;
  painter.scale(scale, scale);

  float mtx34[12];
  for (u32 i = 0; i < 12; i++) {
    mtx34[i] = read_float(p_matrix + i * 4);
  }
  double Xx = mtx34[0], Yx = mtx34[1], Zx = mtx34[2], Cx = mtx34[3];
  double Xy = mtx34[4], Yy = mtx34[5], Zy = mtx34[6];
  double Xz = mtx34[8], Yz = mtx34[9], Zz = mtx34[10], Cz = mtx34[11];

  // 回転角度を求める
  // README.md参照
  double cosine = (Yy + Yz * Yz / (1 + Yy)) * Xx + -Yx * Xy - Yx * Yz / (1 + Yy) * Xz;
  double sine = -Yx * Yz / (1 + Yy) * Xx - Yz * Xy + (Yy + Yx * Yx / (1 + Yy)) * Xz;
  double theta = acos(cosine);
  if (sine < 0)
    theta *= -1;
  double theta_degree = theta / (2 * M_PI) * 360;

  painter.drawText(QRect(0, 0, 200, 200), QString::number(theta_degree));

  QPointF polygon[20];
  for (s64 i = 0; i < 20; i++) {
    double x = radius * cos(i * M_PI / 10.0);
    double z = radius * sin(i * M_PI / 10.0);
    polygon[i] = QPointF(
      x * Xx + mirror_Y * Yx + z * Zx + Cx,
      x * Xz + mirror_Y * Yz + z * Zz + Cz
    );
  }

  // (Cx, Cz)を画面の中心に
  painter.resetTransform();
  scale = side / (radius * 2 + 200);
  painter.translate(-Cx * scale + width() / 2.0, -Cz * scale + height() / 2.0);
  painter.scale(scale, scale);

  QPen pen = QPen();
  pen.setWidth(16);
  painter.setPen(pen);
  painter.drawPolygon(polygon, 20);
  painter.drawLine(polygon[0], polygon[10]);
  painter.drawLine(polygon[5], polygon[15]);

  // チュウハナのレールの描画
  painter.drawPolygon(chuuhana_rail, std::size(chuuhana_rail));
  for (s64 i = 0; i < std::size(chuuhana_rail); i++) {
    QFont font = painter.font();
    font.setPixelSize(200);
    painter.setFont(font);
    double dx = chuuhana_rail[i].x() - Cx;
    double dz = chuuhana_rail[i].y() - Cz;
    double cosine = dx / sqrt(dx * dx + dz * dz);
    double sine = dz / sqrt(dx * dx + dz * dz);
    painter.drawText(chuuhana_rail[i].x() - 50 + 100.0 * cosine, chuuhana_rail[i].y() + 50 + 100.0 * sine, QString::number(i));
  }

  // チュウハナ
  float sum_x = 0, sum_z = 0;
  float ax = 0, az = 0;
  for (u32 id : chuuhana_ids) {
    const u32 p_chuuhana = read_u32(0x81097ce0 + id * 4);
    if (read_u32(p_chuuhana + 0xf0) & 1) {
      continue;
    }
    float x = read_float(p_chuuhana + 0x10);
    float z = read_float(p_chuuhana + 0x18);
    sum_x += x, sum_z += z;
    float degree = -read_float(p_chuuhana + 0x34) + 90.0;
    double cosine = cos(M_PI * degree / 180.0);
    double sine = sin(M_PI * degree / 180.0);
    float target_x = read_float(p_chuuhana + 0x108);
    float target_z = read_float(p_chuuhana + 0x110);
    s32 timer = read_s32(p_chuuhana + 0x1a4);
    s16 collide_count = read_s16(p_chuuhana + 0x48);
    u32 p_collide_objects = read_u32(p_chuuhana + 0x44);
    bool is_collid_move = false;
    for (u32 i = 0; i < collide_count; i++) {
      u32 hit_object = read_u32(p_collide_objects + i * 4);
      if (read_u32(hit_object) == 0x803DAE44) {
        s16 hit_id = read_s16(hit_object + 0x7c);
        is_collid_move = (id < hit_id);
      }
    }

    ax += (x - Cx) / abs(radius * Xx) - Yx;
    az += (z - Cz) / abs(radius * Zz) - Yz;

    pen.setColor(Qt::darkGreen);
    painter.setPen(pen);
    painter.drawLine(x, z, target_x, target_z);
    painter.drawEllipse(target_x - 100, target_z - 100, 200, 200);

    QBrush brush = QBrush();
    if (timer == 400) {
      // willFall
      brush.setColor(Qt::cyan);
      brush.setStyle(Qt::SolidPattern);
      chuuhana_[id].elapsed_time_target_changed = elapsed_timer_.elapsed();
      chuuhana_[id].target_type = ChuuHana::WILL_FALL;
    } else if (float dx = x - target_x, dz = z - target_z; sqrt(dx * dx + dz * dz) < 100.0f) {
      // setGoal
      brush.setColor(Qt::green);
      brush.setStyle(Qt::SolidPattern);
      chuuhana_[id].elapsed_time_target_changed = elapsed_timer_.elapsed();
      chuuhana_[id].target_type = ChuuHana::SET_GOAL;
    } else if (is_collid_move) {
      // CollidMove
      brush.setColor(Qt::lightGray);
      brush.setStyle(Qt::SolidPattern);
      chuuhana_[id].elapsed_time_target_changed = elapsed_timer_.elapsed();
      chuuhana_[id].target_type = ChuuHana::COLLID_MOVE;
    } else if (s64 dt = elapsed_timer_.elapsed() - chuuhana_[id].elapsed_time_target_changed; dt < 1000) {
      QColor color = QColor();
      switch (chuuhana_[id].target_type) {
      case ChuuHana::WILL_FALL:
        color = Qt::cyan;
        break;
      case ChuuHana::SET_GOAL:
        color = Qt::green;
        break;
      case ChuuHana::COLLID_MOVE:
        color = Qt::lightGray;
        break;
      }
      color.setAlphaF(color.alphaF() * (1.0 - dt / 1000.0));
      brush.setColor(color);
      brush.setStyle(Qt::SolidPattern);
    }
    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawEllipse(x - 225, z - 225, 450, 450);
    painter.drawLine(x, z, x + 225 * cosine, z + 225 * sine);
    painter.setBrush(Qt::NoBrush);

    QFont font = painter.font();
    font.setPixelSize(200);
    painter.setFont(font);
    painter.drawText(x + 30, z - 30, QString::number(id));
  }

  // 重心
  pen.setColor(Qt::red);
  painter.setPen(pen);
  u32 chuuhana_count = read_u32(p_mirror + 0x19c);
  if (chuuhana_count)
    painter.drawPoint(sum_x / chuuhana_count, sum_z / chuuhana_count);

  // 鏡の傾き
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  painter.drawLine(Cx, Cz, Cx + Yx * radius * 2, Cz + Yz * radius * 2);

  // 鏡の速度
  pen.setColor(Qt::darkMagenta);
  painter.setPen(pen);
  float vx = read_float(p_mirror + 0x14c);
  float vz = read_float(p_mirror + 0x154);
  painter.drawLine(Cx, Cz, Cx + vx * radius * 10, Cz + vz * radius * 10);

  // 実際の加速度
  pen.setColor(Qt::red);
  painter.setPen(pen);
  painter.drawLine(Cx, Cz, Cx + ax * 100, Cz + az * 100);

  // マリオ
  const u32 p_mario = read_u32(0x8040a378);
  float mario_x = read_float(p_mario + 0x10);
  float mario_z = read_float(p_mario + 0x18);
  u16 mario_spin_angle = read_u16(p_mario + 0x9a);
  QTransform transform_inv = painter.transform().inverted();
  const QRectF bound(transform_inv.mapRect(QRectF(0, 0, width(), height())));
  if (mario_x < bound.left() || bound.right() < mario_x || mario_z < bound.top() || bound.bottom() < mario_z) {
    double tan_xz = (mario_z - Cz) / (mario_x - Cx);
    double tan_zx = (mario_x - Cx) / (mario_z - Cz);
    if (mario_x < Cx)
      tan_xz *= -1;
    if (mario_z < Cz)
      tan_zx *= -1;

    mario_z = tan_xz * (bound.right() - Cx) + Cz;
    if (mario_z < bound.top())
      mario_z = bound.top();
    else if (bound.bottom() < mario_z)
      mario_z = bound.bottom();

    mario_x = tan_zx * (bound.bottom() - Cz) + Cx;
    if (mario_x < bound.left())
      mario_x = bound.left();
    else if (bound.right() < mario_x)
      mario_x = bound.right();
  }
  QImage img_mario(":sms/mario.png");
  painter.translate(mario_x, mario_z);
  painter.rotate(-mario_spin_angle / 65536.0 * 360.0 - 180.0);
  painter.translate(-mario_x, -mario_z);
  painter.drawImage(QRectF(mario_x - 100, mario_z - 100, 200, 200), img_mario);

  // willFall呼び出し半径
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  painter.setTransform(QTransform(scale * Xx, scale * Xz, scale * Zx, scale * Zz, scale * mirror_Y * Yx + width() / 2.0, scale * mirror_Y * Yz + height() / 2.0));
  double r = sqrt(willfall_r * willfall_r - mirror_Y * mirror_Y);
  painter.drawEllipse(-r, -r, r * 2, r * 2);
}