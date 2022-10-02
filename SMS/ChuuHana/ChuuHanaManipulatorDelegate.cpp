#include "ChuuHanaManipulatorDelegate.h"
#include <QSpinBox>

#include "ChuuHanaManipulatorModel.h"

ChuuHanaManipulatorDelegate::ChuuHanaManipulatorDelegate(QObject* parent) : QStyledItemDelegate(parent) {
  
}

QWidget* ChuuHanaManipulatorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto* editor = new SpinBox7a5b(parent);
  return editor;
}

void ChuuHanaManipulatorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  const auto* model = dynamic_cast<const ChuuHanaManipulatorModel*>(index.model());
  const auto* result = &model->rng_->results_.at(index.row());

  auto* spin_box = dynamic_cast<SpinBox7a5b*>(editor);
  spin_box->setValue7a5b(result->index_7a, result->index_5b);
}


void ChuuHanaManipulatorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
  const auto* spin_box = dynamic_cast<SpinBox7a5b*>(editor);
  u32 a, b;
  spin_box->value7a5b(&a, &b);

  auto* model_chuuhana = dynamic_cast<const ChuuHanaManipulatorModel*>(model);
  auto* result = &model_chuuhana->rng_->results_.at(index.row());
  result->index_7a = a;
  result->index_5b = b;
}
