#include "MapObjectParameters.h"

MapObjectParametersModel::MapObjectParametersModel(const std::vector<std::unique_ptr<ParamsBase>> &params_list, QObject* parent)
  : params_list_(params_list){}

//MapObjectParametersModel::MapObjectParametersModel(QObject* parent) {}


s32 MapObjectParametersModel::columnCount(const QModelIndex& parent) const {
  return static_cast<s32>(params_list_.size());
}

s32 MapObjectParametersModel::rowCount(const QModelIndex& parent) const {
  if (params_list_.empty())
    return 0;
  return params_list_.front()->row();
}

QVariant MapObjectParametersModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (params_list_.empty())
    return {};
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      return params_list_[section]->name();
    }
    if (orientation == Qt::Vertical) {
      return params_list_.front()->header_data(section);
    }
  }
  return {};
}

QVariant MapObjectParametersModel::data(const QModelIndex& index, int role) const {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return {};
  if (!index.isValid())
    return {};

  const auto* params = params_list_[index.column()].get();

  if (role == Qt::DisplayRole) {
    return params->data(index.row());
  }

  if (role == Qt::ForegroundRole) {
    return params->data_color();
  }

  if (role == Qt::BackgroundRole && index.column() == selected_column_) {
    return QBrush(QColor(0xddebf7));
  }

  //if (role == Qt::CheckStateRole) {
  //  return true;
  //}

  return {};
}

Qt::ItemFlags MapObjectParametersModel::flags(const QModelIndex& index) const {
  auto flags = QAbstractTableModel::flags(index);
  //flags |= Qt::ItemIsUserCheckable;
  return flags;
}
