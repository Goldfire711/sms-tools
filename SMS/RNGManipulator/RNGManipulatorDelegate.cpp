#include "RNGManipulatorDelegate.h"
#include <QSpinBox>

#include "RNGManipulatorModel.h"
#include "RNGSpinBox7a5b.h"

RNGManipulatorDelegate::RNGManipulatorDelegate(QObject* parent) : QStyledItemDelegate(parent) {
  
}

QWidget* RNGManipulatorDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const {
  auto* editor = new RNGSpinBox7a5b(parent);
  return editor;
}

void RNGManipulatorDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const {
  const auto* model = dynamic_cast<const RNGManipulatorModel*>(index.model());
  const auto* result = &model->rng_->results_.at(index.row());

  auto* spin_box = dynamic_cast<RNGSpinBox7a5b*>(editor);
  spin_box->setValue7a5b(result->index_7a, result->index_5b);
}

void RNGManipulatorDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const {
  const auto* spin_box = dynamic_cast<RNGSpinBox7a5b*>(editor);
  u32 a, b;
  spin_box->value7a5b(&a, &b);

  auto* rng_model = dynamic_cast<const RNGManipulatorModel*>(model);
  auto* result = &rng_model->rng_->results_.at(index.row());
  result->index_7a = a;
  result->index_5b = b;
}
