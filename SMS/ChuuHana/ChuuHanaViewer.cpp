#include "ChuuHanaViewer.h"

#include <QDebug>
#include <QtMath>
#include <Memory/Memory.h>
#include <QPainter>
#include <QMatrix4x3>

using namespace memory;

ChuuHanaViewer::ChuuHanaViewer(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);
}

ChuuHanaViewer::~ChuuHanaViewer() {
}

void ChuuHanaViewer::paintEvent(QPaintEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  const s8 current_stage = read_s8(0x805f8749);
  const s8 current_episode = read_s8(0x805f874a);
  if (current_stage != 4 || current_episode != 1)
    return;

  //const u32 p_mirrorL = read_u32(0x8134945c);
  float mtx34[12];
  for (u32 i = 0; i < 12; i++) {
    mtx34[i] = read_float(0x8134A2F8 + i * 4);
  }
  QPointF polygon_local[20];
  for (s64 i = 0; i < 20; i++) {
    polygon_local[i] = QPointF(1500 * qCos(i * M_PI / 10.0), 1500 * qSin(i * M_PI / 10.0));
  }

  const int side = qMin(width(), height());
  double cx = -2536.25;
  double cy = -6877.95;
  double scale = side / 3200.0;

  QPointF polygon[20];
  for (s64 i = 0; i < 20; i++) {
    polygon[i] = QPointF(
      polygon_local[i].x() * mtx34[0] + 510.0 * mtx34[1] + polygon_local[i].y() * mtx34[2] + mtx34[3],
      polygon_local[i].x() * mtx34[8] + 510.0 * mtx34[9] + polygon_local[i].y() * mtx34[10] + mtx34[11]
    );
  }

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.translate(-cx * scale + width()/2.0, -cy * scale + height()/2.0);
  painter.scale(scale, scale);

  QPen pen = QPen();
  pen.setWidth(16);
  painter.setPen(pen);
  painter.drawPolygon(polygon, 20);
  painter.drawLine(polygon[0], polygon[10]);
  painter.drawLine(polygon[5], polygon[15]);

  // 回転角度を求める
  // 行列のy軸成分を(0,1,0)に戻す回転行列をx軸成分に掛ける
  double yx = mtx34[1], yy = mtx34[5], yz = mtx34[9];
  double xx = mtx34[0], xy = mtx34[4], xz = mtx34[8];
  double cosine = (yy + yz * yz / (1 + yy)) * xx + -yx * xy - yx * yz / (1 + yy) * xz;
  double sine = -yx * yz / (1 + yy) * xx - yz * xy + (yy + yx * yx / (1 + yy)) * xz;
  double theta = acos(cosine);
  if (sine < 0)
    theta *= -1;
  double theta_degree = theta / (2 * M_PI) * 360;
  ui.label_degree->setText(QString::number(theta_degree));

  // チュウハナのレール
  QPoint chuuhana_rail[7] = {
    QPoint(-3300, -7000),
    QPoint(-3200, -6400),
    QPoint(-2800, -6100),
    QPoint(-2122, -6106),
    QPoint(-1700, -6800),
    QPoint(-2099, -7661),
    QPoint(-2900, -7600)
  };
  painter.drawPolygon(chuuhana_rail, 7);

  // willFall呼び出し半径
  painter.drawEllipse(cx - 1100, cy - 1100, 2200, 2200);

  // チュウハナ
  u32 p_chuuhana_manager = read_u32(0x81097c40 + 0x18);
  u32 p_chuuhana[3];
  for (u32 i = 0; i < 3; i++) {
    p_chuuhana[i] = read_u32(p_chuuhana_manager + (i + 3) * 4);
  }
  float sum_x = 0, sum_z = 0;
  QPointF chuuhana_points[3];
  for (u32 i = 0; i < 3; i++) {
    float x = read_float(p_chuuhana[i] + 0x10);
    float z = read_float(p_chuuhana[i] + 0x18);
    sum_x += x, sum_z += z;
    float degree = -read_float(p_chuuhana[i] + 0x34) + 90.0;
    float target_x = read_float(p_chuuhana[i] + 0x108);
    float target_z = read_float(p_chuuhana[i] + 0x110);

    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawLine(x, z, target_x, target_z);

    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawEllipse(x - 225, z - 225, 450, 450);
    painter.drawEllipse(x - 100, z - 100, 200, 200);
    painter.drawLine(x, z, x + 225 * cos(M_PI * degree / 180.0), z + 225 * sin(M_PI * degree / 180.0));
  }

  // 重心
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  painter.drawPoint(sum_x / 3.0, sum_z / 3.0);
}
