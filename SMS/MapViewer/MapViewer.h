#pragma once

#include <QWidget>
#include <QMainWindow>
#include <QCheckBox>
#include <QPushButton>
#include "MapPinnaBeach.h"
#include "MapGeneral.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <json.hpp>
#include <fstream>
#include <QValidator>

class MapViewer : public QMainWindow {
Q_OBJECT

public:
  MapViewer(QWidget* parent = Q_NULLPTR);
  ~MapViewer() override;

private:
  MapGeneral* map_;
};
