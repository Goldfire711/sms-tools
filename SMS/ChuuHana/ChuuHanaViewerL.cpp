#include "ChuuHanaViewerL.h"

#include <QDebug>
#include <QtMath>
#include <Memory/Memory.h>
#include <QPainter>
#include <QMatrix4x3>
#include <QVBoxLayout>

using namespace memory;

ChuuHanaViewerL::ChuuHanaViewerL(QWidget* parent)
  : QWidget(parent) {
}

ChuuHanaViewerL::~ChuuHanaViewerL() {
}

void ChuuHanaViewerL::paintEvent(QPaintEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  const s8 current_stage = read_s8(0x805f8749);
  const s8 current_episode = read_s8(0x805f874a);
  if (current_stage != 4 || current_episode != 1)
    return;

  // MirrorL�̏��
  constexpr u32 p_mirror = 0x8134945c;
  constexpr u32 p_matrix = 0x8134A2F8;
  constexpr double radius = 1500.0;
  constexpr double mirror_Y = 510.0;
  constexpr u32 chuuhana_ids[] = { 3, 4, 5 };
  constexpr double willfall_r = 1100.0;
  QPoint chuuhana_rail[] = {
    QPoint(-3300, -7000),
    QPoint(-3200, -6400),
    QPoint(-2800, -6100),
    QPoint(-2122, -6106),
    QPoint(-1700, -6800),
    QPoint(-2099, -7661),
    QPoint(-2900, -7600)
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

  // ��]�p�x�����߂�
  // �s���(Yx,Yy,Yz)��(0,1,0)�ɖ߂���]�s���(Xx,Xy,Xz)�Ɋ|�����(cos��,0,sin��)���o��
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

  // (Cx, Cz)����ʂ̒��S��
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

  // �`���E�n�i�̃��[���̕`��
  painter.drawPolygon(chuuhana_rail, std::size(chuuhana_rail));

  // �`���E�n�i
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

    ax += (x - Cx) / abs(radius * Xx) - Yx;
    az += (z - Cz) / abs(radius * Zz) - Yz;

    pen.setColor(Qt::darkGreen);
    painter.setPen(pen);
    painter.drawLine(x, z, target_x, target_z);
    painter.drawEllipse(target_x - 100, target_z - 100, 200, 200);

    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawEllipse(x - 225, z - 225, 450, 450);
    painter.drawLine(x, z, x + 225 * cosine, z + 225 * sine);

    QFont font = painter.font();
    font.setPixelSize(200);
    painter.setFont(font);
    painter.drawText(x + 30, z - 30, QString::number(id));
  }

  // �d�S
  pen.setColor(Qt::red);
  painter.setPen(pen);
  u32 chuuhana_count = read_u32(p_mirror + 0x19c);
  if (chuuhana_count)
    painter.drawPoint(sum_x / chuuhana_count, sum_z / chuuhana_count);

  // ���̌X��
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  painter.drawLine(Cx, Cz, Cx + Yx * radius * 2, Cz + Yz * radius * 2);

  // ���̑��x
  pen.setColor(Qt::darkMagenta);
  painter.setPen(pen);
  float vx = read_float(p_mirror + 0x14c);
  float vz = read_float(p_mirror + 0x154);
  painter.drawLine(Cx, Cz, Cx + vx * radius * 10, Cz + vz * radius * 10);

  // ���ۂ̉����x
  pen.setColor(Qt::red);
  painter.setPen(pen);
  painter.drawLine(Cx, Cz, Cx + ax * 100, Cz + az * 100);

  // �}���I
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

  // willFall�Ăяo�����a
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  painter.setTransform(QTransform(scale * Xx, scale * Xz, scale * Zx, scale * Zz, scale * mirror_Y* Yx + width() / 2.0, scale * mirror_Y* Yz + height() / 2.0));
  double r = sqrt(willfall_r * willfall_r - mirror_Y * mirror_Y);
  painter.drawEllipse(-r, -r, r * 2, r * 2);

  // Debug
  //painter.resetTransform();
  //painter.setPen(QPen());
  //painter.setFont(QFont());
  //painter.scale(side / 200.0, side / 200.0);
  //painter.drawText(QRectF(0, 20, 400, 200), debug);
}
