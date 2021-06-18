#pragma once

#include <QWidget>
#include <QTimer>
#include "ui_Spin.h"

class Spin : public QWidget {
Q_OBJECT

public:
  Spin(QWidget* parent = Q_NULLPTR);
  ~Spin() override;

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  Ui::Spin ui;
};
