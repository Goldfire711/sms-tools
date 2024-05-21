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
#include <QMenuBar>
#include <QComboBox>

#include "Map/MapGeneral.h"
#include "MapObjectParameters/MapObjectParameters.h"
#include "MapObjectViewer/MapObjectViewer.h"
#include "Settings/Settings.h"
#include "SMS/ObjectViewer/ObjectParameters.h"

class MapViewer : public QMainWindow {
Q_OBJECT

public:
  MapViewer(QWidget* parent = Q_NULLPTR);
  ~MapViewer() override;

private:
  MapGeneral* map_;
  MapObjectViewer* object_viewer_;
  MapObjectParameters* object_watcher_;
  ObjectParameters* object_parameters_;
};
