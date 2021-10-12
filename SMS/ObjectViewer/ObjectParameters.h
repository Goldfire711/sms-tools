#pragma once

#include <QWidget>
#include "ui_ObjectParameters.h"
#include "../../Common/CommonTypes.h"

class ObjectParameters : public QWidget {
Q_OBJECT

public:
  ObjectParameters(QWidget* parent = Q_NULLPTR);
  ~ObjectParameters() override;
  void set_label(QString string);
  void show_parameters(u32 address, s64 index);

private:
  Ui::ObjectParameters ui;
};
