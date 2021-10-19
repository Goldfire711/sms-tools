#include "ObjectParametersModel.h"
#include "../../DolphinProcess/DolphinAccessor.h"

ObjectParametersModel::ObjectParametersModel(QVector<ObjectParametersItem>* items, QObject* parent)
  : QAbstractTableModel(parent) {
  items_ = items;
}

ObjectParametersModel::~ObjectParametersModel() {
}

int ObjectParametersModel::columnCount(const QModelIndex& parent) const {
  return COLUMN_COUNT;
}

int ObjectParametersModel::rowCount(const QModelIndex& parent) const {
  return items_->size();
}

QVariant ObjectParametersModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();
  if (role != Qt::DisplayRole)
    return QVariant();
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return QVariant();

  const auto item = items_->at(index.row());
  switch (index.column()) {
  case COLUMN_OFFSET:
    return "0x" + QString::number(item.base_offset_ + item.offset_, 16);
  case COLUMN_TYPE:
    return item.string_type_;
  case COLUMN_NAME:
    return item.name_;
  case COLUMN_VALUE:
    return item.string_value_;
  case COLUMN_CLASS:
    return item.class_name_;
  default:
    return QVariant();
  }
}

QVariant ObjectParametersModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
    return QVariant();
  switch (section) {
  case COLUMN_OFFSET:
    return tr("Offset");
  case COLUMN_TYPE:
    return tr("Type");
  case COLUMN_NAME:
    return tr("Name");
  case COLUMN_VALUE:
    return tr("Value");
  case COLUMN_CLASS:
    return tr("Class");
  default:
    return QVariant();
  }
}

void ObjectParametersModel::update_list() {
  emit layoutChanged();
}
