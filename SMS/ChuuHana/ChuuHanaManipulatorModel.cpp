#include "ChuuHanaManipulatorModel.h"

ChuuHanaManipulatorModel::ChuuHanaManipulatorModel(QObject *parent, ChuuHanaRNG* rng)
    : QAbstractTableModel(parent), rng_(rng) {
}

ChuuHanaManipulatorModel::~ChuuHanaManipulatorModel() {
}

int ChuuHanaManipulatorModel::columnCount(const QModelIndex& parent) const {
  return COL_COUNT;
}

int ChuuHanaManipulatorModel::rowCount(const QModelIndex& parent) const {
  return static_cast<int>(rng_->results_.size());
}

QVariant ChuuHanaManipulatorModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case COL_INDEX:
      return "Index+";
    case COL_7A5B_INDEX:
      return "Drops(7a+5b)";
    case COL_VALUE:
      return "RNG Value";
    case COL_SEED:
      return "RNG Seed";
    }
  }
  return QVariant();
}

QVariant ChuuHanaManipulatorModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    QString str_7a5b;
    const Result* result = &rng_->results_.at(index.row());
    switch (index.column()) {
    case COL_INDEX:
      return result->index;
    case COL_7A5B_INDEX:
      if (result->has_7a5b_index) {
        str_7a5b.append("(");
        str_7a5b.append(QString::number(result->index_7a));
        str_7a5b.append(" ");
        str_7a5b.append(QString::number(result->index_5b));
        str_7a5b.append(")");
      }
      return str_7a5b;
    case COL_VALUE:
      return result->value;
    case COL_SEED:
      return QString::number(result->seed, 16).toUpper();
    }
  }
  return QVariant();
}

Qt::ItemFlags ChuuHanaManipulatorModel::flags(const QModelIndex& index) const {
  constexpr Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (index.column() == COL_7A5B_INDEX && !index.data().toString().isEmpty())
    return flags | Qt::ItemIsEditable;
  return flags;
}

void ChuuHanaManipulatorModel::update_list() {
  emit layoutChanged();
}
