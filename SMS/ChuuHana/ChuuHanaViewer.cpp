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
}
