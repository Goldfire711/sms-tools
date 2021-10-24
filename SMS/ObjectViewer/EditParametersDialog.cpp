#include "EditParametersDialog.h"
#include <QJsonDocument>
#include <QJsonObject>

EditParametersDialog::EditParametersDialog(QJsonDocument& json_classes, QWidget* parent)
  : QDialog(parent) {
  ui.setupUi(this);

  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

  auto json_chuuhana = json_classes["_default"].toObject();
  model_ = new EditParametersModel(json_chuuhana, this);
  ui.table_parameters->setModel(model_);
  ui.table_parameters->resizeColumnsToContents();
  resize(500, 600);
}

EditParametersDialog::~EditParametersDialog() {
}