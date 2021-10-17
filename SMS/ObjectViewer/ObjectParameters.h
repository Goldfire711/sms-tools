#pragma once

#include <QWidget>
#include <QJsonDocument>
#include "ui_ObjectParameters.h"
#include "../../Common/CommonTypes.h"
#include "ObjectParametersItem.h"
#include "ObjectParametersModel.h"

class ObjectParameters : public QWidget {
Q_OBJECT

public:
  ObjectParameters(QWidget* parent = Q_NULLPTR);
  ~ObjectParameters() override;
  void set_label(QString string);
  void show_parameters(u32 address, s64 index);
  void refresh_items();

private:
  Ui::ObjectParameters ui;
  u32 address_;
  QVector<ObjectParametersItem> items_;
  QJsonDocument json_classes_;
  ObjectParametersModel* model_;

  void load_items_from_json(const QJsonObject& json, QString class_name);
};
