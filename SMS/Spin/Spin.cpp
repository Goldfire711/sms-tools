#include "Spin.h"
#include "../../Memory/Memory.h"

#include <QPainter>
#include <QtMath>
#include <Windows.h>

using namespace memory;

Spin::Spin(QWidget* parent) : QWidget(parent) {
  ui.setupUi(this);
}

Spin::~Spin() {
}

void Spin::paintEvent(QPaintEvent* event) {
  const int side = qMin(width(), height());

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.translate(width() / 2, height() / 2);
  painter.scale(side / 200.0, side / 200.0);

  painter.setPen(Qt::gray);
  painter.setBrush(Qt::NoBrush);

  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  // read memory
  const s16 camera_angle = read_s16(0x8040b370, {0x258});
  const u32 ptr_angles = read_u32(0x8040a378);
  const s16 current_facing_angle = read_s16(ptr_angles + 0x96);
  const s16 current_stick_angle = read_s16(ptr_angles + 0x90);
  const s16 current_spin_angle = read_s16(ptr_angles + 0x9a);
  const u8 stick_count = read_u8(ptr_angles + 0x534);
  const u32 ptr_stick_angles = read_u32(ptr_angles + 0x530);
  const auto stick_angles = new s16[stick_count];
  for (s64 i = 0; i < stick_count; i++) {
    stick_angles[i] = read_s16(ptr_stick_angles + i * 2);
  }

  painter.rotate(360.0 * (camera_angle / 65536.0));

  bool nwse[4] = {false};
  s64 nwse_last_index[4] = {-1};
  QVector<QLineF> stick_lines;
  stick_lines.resize(stick_count);
  for (s64 i = 0; i < stick_count; i++) {
    if (stick_angles[i] < -24576 || 24576 < stick_angles[i]) {
      nwse[0] = true;
      nwse_last_index[0] = i;
    } else if (-24576 <= stick_angles[i] && stick_angles[i] <= -8192) {
      nwse[1] = true;
      nwse_last_index[1] = i;
    } else if (-8192 < stick_angles[i] && stick_angles[i] < 8192) {
      nwse[2] = true;
      nwse_last_index[2] = i;
    } else if (8192 <= stick_angles[i] && stick_angles[i] <= 24576) {
      nwse[3] = true;
      nwse_last_index[3] = i;
    }
    const double stick_radius = -2 * M_PI * (stick_angles[i] / 65536.0);
    stick_lines[i] = QLineF(0, 0, -70 * sin(stick_radius), -70 * -cos(stick_radius));
  }

  const auto circle = QRectF(-70, -70, 70 * 2, 70 * 2);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QColor(0xdcdcdc));
  if (nwse[0]) {
    painter.drawPie(circle, 45 * 16, 90 * 16);
  }
  if (nwse[1]) {
    painter.drawPie(circle, 135 * 16, 90 * 16);
  }
  if (nwse[2]) {
    painter.drawPie(circle, 225 * 16, 90 * 16);
  }
  if (nwse[3]) {
    painter.drawPie(circle, 315 * 16, 90 * 16);
  }
  if (!(nwse[0] == false || nwse[1] == false || nwse[2] == false || nwse[3] == false)) {
    const auto* min_last_index = std::min_element(nwse_last_index, nwse_last_index + 4);
    const auto spin_frame = ((39 - stick_count) + (*min_last_index + 1)) / 4.0;
    ui.label_spin_frame->setText(QString::number(spin_frame) + "f");
  } else {
    ui.label_spin_frame->setText("");
  }

  const double current_stick_radius = -2 * M_PI * (current_stick_angle / 65536.0);
  const double current_facing_radius = -2 * M_PI * (current_facing_angle / 65536.0);
  const double current_spin_radius = -2 * M_PI * (current_spin_angle / 65536.0);
  const auto current_stick_line =
    QLineF(0, 0, -70 * sin(current_stick_radius), -70 * -cos(current_stick_radius));
  const auto current_facing_line =
    QLineF(0, 0, -70 * sin(current_facing_radius), -70 * -cos(current_facing_radius));
  const auto current_spin_line =
    QLineF(0, 0, -70 * sin(current_spin_radius), -70 * -cos(current_spin_radius));

  painter.setBrush(Qt::NoBrush);
  painter.setPen(QColor(0xf5f5f5));
  painter.drawLines(stick_lines);

  painter.setPen(Qt::darkGreen);
  painter.drawLine(current_spin_line);

  painter.setPen(Qt::blue);
  painter.drawLine(current_stick_line);

  painter.setPen(Qt::red);
  painter.drawLine(current_facing_line);

  painter.setPen(Qt::black);
  painter.drawEllipse(circle);
  painter.setPen(Qt::gray);
  painter.drawLine(-70 * qCos(M_PI_4), -70 * qSin(M_PI_4), 70 * qCos(M_PI_4), 70 * qSin(M_PI_4));
  painter.drawLine(-70 * qCos(M_PI_2 + M_PI_4), -70 * qSin(M_PI_2 + M_PI_4),
                   70 * qCos(M_PI_2 + M_PI_4), 70 * qSin(M_PI_2 + M_PI_4));

  painter.setPen(Qt::red);
  painter.drawText(QRectF(-10, -90, 20, 20), Qt::AlignCenter, tr("N"));

  delete[] stick_angles;
}
