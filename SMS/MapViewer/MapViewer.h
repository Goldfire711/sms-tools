#pragma once

#include <QWidget>
#include <QCheckBox>
#include <QPushButton>
#include "MapPinnaBeach.h"

class MapViewer : public QWidget {
Q_OBJECT

public:
  MapViewer(QWidget* parent = Q_NULLPTR);
  ~MapViewer() override;

private:
  MapPinnaBeach* map_pinna_;
  QCheckBox* chb_center_on_;
  QPushButton* btn_capture_;

  void button_capture_clicked();
};
