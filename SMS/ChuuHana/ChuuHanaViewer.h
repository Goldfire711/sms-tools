#pragma once

#include <QWidget>
#include "ui_ChuuHanaViewer.h"

class ChuuHanaViewer : public QWidget {
  Q_OBJECT

public:
  ChuuHanaViewer(QWidget* parent = Q_NULLPTR);
  ~ChuuHanaViewer() override;

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  Ui::ChuuHanaViewer ui;
};
