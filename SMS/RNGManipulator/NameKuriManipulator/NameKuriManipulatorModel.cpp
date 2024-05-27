#include "NameKuriManipulator.h"

NameKuriManipulatorModel::NameKuriManipulatorModel(QObject* parent, NameKuriRNG* rng)
  : QAbstractTableModel(parent), rng_(rng) {
}

int NameKuriManipulatorModel::columnCount(const QModelIndex& parent) const {
  return 7;
}

int NameKuriManipulatorModel::rowCount(const QModelIndex& parent) const {
  return static_cast<int>(rng_->results_.size());
}

QVariant NameKuriManipulatorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return NameKuriRNG::header_data(section);
  }
  return {};
}

QVariant NameKuriManipulatorModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return {};

  if (role == Qt::DisplayRole) {
    return rng_->data(index.row(), index.column());
  }
  return {};
}

Qt::ItemFlags NameKuriManipulatorModel::flags(const QModelIndex& index) const {
  constexpr Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (index.column() == 1 && !index.data().toString().isEmpty())
    return flags | Qt::ItemIsEditable;
  return flags;
}

void NameKuriManipulatorModel::update_list() {
  emit layoutChanged();
}
