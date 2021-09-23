#pragma once

#include <QWidget>
#include "ui_RNGManipulator.h"

class RNGManipulator : public QWidget {
Q_OBJECT

public:
  RNGManipulator(QWidget* parent = Q_NULLPTR);
  ~RNGManipulator() override;

private:
  Ui::RNGManipulator ui;
};
