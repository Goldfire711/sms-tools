#pragma once

#include <QStyledItemDelegate>

#include "Common/CommonTypes.h"

class RNGManipulatorDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  RNGManipulatorDelegate(QObject* parent = nullptr);
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};
