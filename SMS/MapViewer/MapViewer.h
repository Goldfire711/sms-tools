#pragma once

#include <QWidget>
#include "ui_MapViewer.h"
#include "MapPinnaBeach.h"

class MapViewer : public QWidget {
Q_OBJECT

public:
  MapViewer(QWidget* parent = Q_NULLPTR);
  ~MapViewer() override;

private:
  Ui::MapViewer ui;
  MapPinnaBeach* map_pinna_;

  void center_on_clicked();
};
