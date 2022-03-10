#pragma once

#include <QWidget>
#include "ui_ObjectSubParameters.h"
#include <json.hpp>
#include "../../Common/CommonTypes.h"
#include "ObjectParametersItem.h"
#include "ObjectParametersModel.h"

class ObjectSubParameters : public QWidget {
  Q_OBJECT

public:
  ObjectSubParameters(QWidget* parent = Q_NULLPTR);
  ~ObjectSubParameters() override;
  void show_parameters(u32 address, const QString& class_name, const QString& name);
  void refresh_items();
  void show_edit_parameters_dialog();
  void reload_json();
  void open_json_location();

private:
  Ui::ObjectSubParameters ui;
  u32 address_;
  QVector<ObjectParametersItem> items_;
  ObjectParametersModel* model_;
  nlohmann::json json_parameters_;
  QString class_name_;
  QString name_;
  ObjectSubParameters* sub_parameters_ = nullptr;

  void read_parameters();
  void load_items_from_json(const nlohmann::json& j, const QString& class_name, u32 base_offset = 0, const QString& parent_name = "");
  void on_table_double_clicked(const QModelIndex& index);
};
