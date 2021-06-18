#pragma once

#include <QWidget>
#include "ui_ObjectViewer.h"
#include "ObjectViewerModel.h"


class ObjectViewer : public QWidget {
  Q_OBJECT

public:
  ObjectViewer(QWidget* parent = Q_NULLPTR);
  ~ObjectViewer() override;
  //void on_update();

protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;

private:
  Ui::ObjectViewer ui;
  //void test();

  ObjectViewerModel* model_;
  s64 number_ = 0;
};
