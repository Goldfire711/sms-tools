#pragma once

#include <QDialog>
#include "ui_EditParametersDialog.h"
#include "EditParametersModel.h"

class EditParametersDialog : public QDialog {
Q_OBJECT

public:
  EditParametersDialog(QJsonDocument& json_classes, QWidget* parent = Q_NULLPTR);
  ~EditParametersDialog() override;

private:
  Ui::EditParametersDialog ui;
  EditParametersModel* model_;
};
