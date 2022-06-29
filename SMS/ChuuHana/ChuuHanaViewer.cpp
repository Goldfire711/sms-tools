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
  double Xx = mtx34[0], Xy = mtx34[4], Xz = mtx34[8];
  double Yx = mtx34[1], Yy = mtx34[5], Yz = mtx34[9];
  double Zx = mtx34[2], Zy = mtx34[6], Zz = mtx34[10];
  double Cx = mtx34[3];
  double Cz = mtx34[11];

  QPointF polygon[20];
  for (s64 i = 0; i < 20; i++) {
    double x = 1500 * cos(i * M_PI / 10.0);
    double z = 1500 * sin(i * M_PI / 10.0);
    polygon[i] = QPointF(
      x * Xx + 510.0 * Yx + z * Zx + Cx,
      x * Xz + 510.0 * Yz + z * Zz + Cz
    );
  }

  const int side = qMin(width(), height());
  double scale = side / 3200.0;

  // (Cx, Cz)を画面の中心に
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.translate(-Cx * scale + width()/2.0, -Cz * scale + height()/2.0);
  painter.scale(scale, scale);

  QPen pen = QPen();
  pen.setWidth(16);
  painter.setPen(pen);
  painter.drawPolygon(polygon, 20);
  painter.drawLine(polygon[0], polygon[10]);
  painter.drawLine(polygon[5], polygon[15]);

  // 回転角度を求める
  // 行列のy軸成分を(0,1,0)に戻す回転行列をx軸成分に掛ける
  double cosine = (Yy + Yz * Yz / (1 + Yy)) * Xx + -Yx * Xy - Yx * Yz / (1 + Yy) * Xz;
  double sine = -Yx * Yz / (1 + Yy) * Xx - Yz * Xy + (Yy + Yx * Yx / (1 + Yy)) * Xz;
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

  // チュウハナ
  u32 p_chuuhana_manager = read_u32(0x81097c40 + 0x18);
  u32 p_chuuhana[3];
  for (u32 i = 0; i < 3; i++) {
    p_chuuhana[i] = read_u32(p_chuuhana_manager + (i + 3) * 4);
  }
  float sum_x = 0, sum_z = 0;
  float ax = 0, az = 0;
  QPointF chuuhana_points[3];
  for (u32 i = 0; i < 3; i++) {
    float x = read_float(p_chuuhana[i] + 0x10);
    float z = read_float(p_chuuhana[i] + 0x18);
    sum_x += x, sum_z += z;
    float degree = -read_float(p_chuuhana[i] + 0x34) + 90.0;
    float target_x = read_float(p_chuuhana[i] + 0x108);
    float target_z = read_float(p_chuuhana[i] + 0x110);

    ax += (x - Cx) / abs(1500 * Xx) - Yx;
    az += (z - Cz) / abs(1500 * Zz) - Yz;

    pen.setColor(Qt::red);
    painter.setPen(pen);
    painter.drawLine(x, z, target_x, target_z);
    painter.drawEllipse(target_x - 100, target_z - 100, 200, 200);

    pen.setColor(Qt::black);
    painter.setPen(pen);
    painter.drawEllipse(x - 225, z - 225, 450, 450);
    painter.drawLine(x, z, x + 225 * cos(M_PI * degree / 180.0), z + 225 * sin(M_PI * degree / 180.0));

    QFont font = painter.font();
    font.setPixelSize(200);
    painter.setFont(font);
    painter.drawText(x + 30, z - 30, QString::number(i + 3));
  }

  // 重心
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  painter.drawPoint(sum_x / 3.0, sum_z / 3.0);

  // 実際の加速度
  painter.drawLine(Cx, Cz, Cx + ax * 100, Cz + az * 100);

  // willFall呼び出し半径
  painter.setTransform(QTransform(scale * Xx, scale * Xz, scale * Zx, scale * Zz, scale * 510 * Yx + width() / 2.0, scale * 510 * Yz + height() / 2.0));
  double r = sqrt(1100.0 * 1100.0 - 510.0 * 510.0);
  painter.drawEllipse(-r, -r, r * 2, r * 2);
}
