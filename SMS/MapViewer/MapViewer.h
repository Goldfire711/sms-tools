#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QCheckBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <json.hpp>
#include <fstream>
#include <QValidator>
#include <QDockWidget>

#include "Map/MapGeneral.h"
#include "MapObjectViewer/MapObjectViewer.h"

class MapViewer : public QMainWindow {
Q_OBJECT

public:
  MapViewer(QWidget* parent = Q_NULLPTR);
  ~MapViewer() override;

private:
  MapGeneral* map_;
  MapObjectViewer* object_viewer_;
};
