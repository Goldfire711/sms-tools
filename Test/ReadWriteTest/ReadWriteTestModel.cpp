#include "ReadWriteTestModel.h"
#include "../../DolphinProcess/DolphinAccessor.h"

ReadWriteTestModel::ReadWriteTestModel(QVector<ReadWriteTestItem> *items, QObject* parent)
  : QAbstractTableModel(parent) {
  items_ = items;
}

ReadWriteTestModel::~ReadWriteTestModel() {
}

int ReadWriteTestModel::columnCount(const QModelIndex& parent) const {
  return COLUMN_COUNT;
}

int ReadWriteTestModel::rowCount(const QModelIndex& parent) const {
  return items_->size();
}

QVariant ReadWriteTestModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();
  if (role != Qt::DisplayRole)
    return QVariant();
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return QVariant();

  const auto item = items_->at(index.row());
  switch (index.column()) {
  case COLUMN_NAME:
    return item.name_;
  case COLUMN_TYPE:
    return item.string_type_;
  case COLUMN_ADDRESS:
    return QString::number(item.address_, 16);
  case COLUMN_VALUE:
    return item.string_value_;
  default:
    return QVariant();
  }
}

QVariant ReadWriteTestModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    return QVariant();
  switch (section) {
  case COLUMN_NAME:
    return tr("name");
  case COLUMN_TYPE:
    return tr("type");
  case COLUMN_ADDRESS:
    return tr("address");
  case COLUMN_VALUE:
    return tr("value");
  default:
    return QVariant();
  }
}

void ReadWriteTestModel::update_list() {
  emit layoutChanged();
}
