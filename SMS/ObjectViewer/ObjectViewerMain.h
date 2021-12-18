#pragma once

#include <QMainWindow>
#include "ui_ObjectViewerMain.h"
#include "ObjectParameters.h"

class ObjectViewerMain : public QMainWindow {
Q_OBJECT

public:
  ObjectViewerMain(QWidget* parent = Q_NULLPTR);
  ~ObjectViewerMain() override;

private:
  Ui::ObjectViewerMain ui;
  ObjectParameters* object_parameters_ = nullptr;

  void show_widget_object_parameters();
};
