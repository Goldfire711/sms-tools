#include "EditParametersModel.h"
#include "ObjectParameters.h"

EditParametersModel::EditParametersModel(QJsonObject& json, QObject* parent)
  : QAbstractTableModel(parent) {
  json_class_ = json;
  json_offsets_ = json["offsets"].toArray();
}

EditParametersModel::~EditParametersModel() {
}

int EditParametersModel::columnCount(const QModelIndex& parent) const {
  return COLUMN_COUNT;
}

int EditParametersModel::rowCount(const QModelIndex& parent) const {
  return json_offsets_.count();
}

QVariant EditParametersModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  const QJsonValue offset = json_offsets_[index.row()];
  if (role == Qt::DisplayRole) {
    switch (index.column()) {
    case COLUMN_OFFSET:
      return "0x" + QString::number(offset["offset"].toString().toUInt(nullptr, 16), 16);
    case COLUMN_NAME:
      return offset["name"].toString();
    case COLUMN_TYPE:
      return offset["type"].toString();
    case COLUMN_SIZE:
      return 0;
    case COLUMN_NOTES:
      return offset["notes"].toString();
    }
  }
  if (role == Qt::CheckStateRole && index.column() == COLUMN_IS_LOCKED) {
    if (offset["is_locked"].toBool())
      return Qt::Checked;
    return Qt::Unchecked;
  }

   return QVariant();
}

bool EditParametersModel::setData(const QModelIndex& index, const QVariant& value, int role) {
  if (!index.isValid())
    return false;

  auto offset = json_offsets_[index.row()];
  auto offset_object = offset.toObject();
  if (role == Qt::EditRole) {
    switch (index.column()) {
    case COLUMN_NAME:
      offset_object["name"] = value.toString();
      offset = offset_object;
      json_class_["offsets"] = json_offsets_;
      emit dataChanged(index, index);
      return true;
    case COLUMN_OFFSET:
      offset_object["offset"] = value.toString();
      offset = offset_object;
      json_class_["offsets"] = json_offsets_;
      emit dataChanged(index, index);
      return true;
    }
  }
  if (role == Qt::CheckStateRole && index.column() == COLUMN_IS_LOCKED) {
    if(value == Qt::Checked) {
      offset_object["is_locked"] = true;
    }
    else {
      offset_object["is_locked"] = false;
    }
    offset = offset_object;
    json_class_["offsets"] = json_offsets_;
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


QVariant EditParametersModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case COLUMN_IS_LOCKED:
      return tr("Lock");
    case COLUMN_OFFSET:
      return tr("Offset");
    case COLUMN_NAME:
      return tr("Name");
    case COLUMN_TYPE:
      return tr("Type");
    case COLUMN_SIZE:
      return tr("Size");
    case COLUMN_NOTES:
      return tr("Notes");
    }
  }
  return QVariant();
}

Qt::ItemFlags EditParametersModel::flags(const QModelIndex& index) const {
  if (!index.isValid())
    return Qt::NoItemFlags;

  const Qt::ItemFlags flags_read_only = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
  const Qt::ItemFlags flags_editable = flags_read_only | Qt::ItemIsEditable;

  auto current_row = json_offsets_[index.row()].toObject();

  if (index.column() == COLUMN_IS_LOCKED)
    return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
  if (index.column() == COLUMN_SIZE)
    return flags_read_only;
  if (index.column() == COLUMN_TYPE || index.column() == COLUMN_NAME | index.column() == COLUMN_NOTES) {
    if (current_row["is_locked"].toBool())
      return flags_read_only;
    return flags_editable;
  }
  return flags_read_only;
}


void EditParametersModel::update_list() {
  emit layoutChanged();
}