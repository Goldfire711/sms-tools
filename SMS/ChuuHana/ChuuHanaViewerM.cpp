#include "ChuuHanaViewerM.h"

#include <QDebug>
#include <QtMath>
#include <Memory/Memory.h>
#include <QPainter>
#include <QMatrix4x3>
#include <QVBoxLayout>

using namespace memory;

ChuuHanaViewerM::ChuuHanaViewerM(QWidget* parent)
  : QWidget(parent) {
}

ChuuHanaViewerM::~ChuuHanaViewerM() {
}

void ChuuHanaViewerM::paintEvent(QPaintEvent* event) {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  const s8 current_stage = read_s8(0x805f8749);
  const s8 current_episode = read_s8(0x805f874a);
  if (current_stage != 4 || current_episode != 1)
    return;

  // MirrorMの情報
  constexpr u32 p_mirror = 0x81345924;
  constexpr u32 p_matrix = 0x813467b8;
  constexpr double radius = 1250.0;
  constexpr double mirror_Y = 416.666717529296875;
  constexpr u32 chuuhana_ids[] = { 1, 2 };
  constexpr double willfall_r = 900.0;
  QPoint chuuhana_rail[] = {
    QPoint(-7500, -2800),
    QPoint(-7300, -2200),
    QPoint(-6700, -2000),
    QPoint(-6200, -2200),
    QPoint(-6100, -2700),
    QPoint(-6200, -3200),
    QPoint(-6800, -3400),
    QPoint(-7200, -3200)
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
  // 行列の(Yx,Yy,Yz)を(0,1,0)に戻す回転行列を(Xx,Xy,Xz)に掛けると(cosθ,0,sinθ)が出る
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

    ax += (x - Cx) / abs(radius * Xx) - Yx;
    az += (z - Cz) / abs(radius * Zz) - Yz;

    pen.setColor(Qt::red);
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

  // 重心
  pen.setColor(Qt::blue);
  painter.setPen(pen);
  u32 chuuhana_count = read_u32(p_mirror + 0x19c);
  if (chuuhana_count)
    painter.drawPoint(sum_x / chuuhana_count, sum_z / chuuhana_count);

  // 実際の加速度
  painter.drawLine(Cx, Cz, Cx + ax * 100, Cz + az * 100);

  // willFall呼び出し半径
  painter.setTransform(QTransform(scale * Xx, scale * Xz, scale * Zx, scale * Zz, scale * mirror_Y * Yx + width() / 2.0, scale * mirror_Y * Yz + height() / 2.0));
  double r = sqrt(willfall_r * willfall_r - mirror_Y * mirror_Y);
  painter.drawEllipse(-r, -r, r * 2, r * 2);
}
