#pragma once

#include <QWidget>
#include <QDockWidget>
#include <QJsonDocument>
#include <json.hpp>
#include "ui_ObjectParameters.h"
#include "../../Common/CommonTypes.h"
#include "ObjectParametersItem.h"
#include "ObjectParametersModel.h"
#include "EditParametersDialog.h"
#include "ObjectSubParameters.h"

class ObjectParameters : public QDockWidget {
  Q_OBJECT

public:
  ObjectParameters(QWidget* parent = Q_NULLPTR);
  ~ObjectParameters() override;
  void closeEvent(QCloseEvent* event) override;
  void show_parameters(u32 address, s64 index);
  void refresh_items();
  void show_edit_parameters_dialog();
  void reload_json();
  void open_json_location();

signals:
  void closed();

private:
  Ui::ObjectParameters ui;
  u32 address_;
  s64 index_;
  QVector<ObjectParametersItem> items_;
  ObjectParametersModel* model_;
  nlohmann::json json_parameters_;
  ObjectSubParameters* sub_parameters_ = nullptr;

  void read_parameters();
  void load_items_from_json(const nlohmann::json& j, const QString& class_name, u32 base_offset = 0, const QString& parent_name = "");
  void on_table_double_clicked(const QModelIndex& index);
};
