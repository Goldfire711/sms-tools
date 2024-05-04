#include "MapObjectViewer.h"

MapObjectViewerModel::MapObjectViewerModel(QObject* parent)
  : QAbstractItemModel(parent)
  , root_item_(std::make_unique<MapObjectViewerItem>()) {}

MapObjectViewerModel::~MapObjectViewerModel() = default;

QModelIndex MapObjectViewerModel::index(s32 row, s32 column, const QModelIndex& parent) const {
  if (!hasIndex(row, column, parent))
    return {};

  MapObjectViewerItem* parent_item = parent.isValid()
    ? static_cast<MapObjectViewerItem*>(parent.internalPointer())
    : root_item_.get();

  if (auto child_item = parent_item->children_[row].get())
    return createIndex(row, column, child_item);
  return {};
}

QModelIndex MapObjectViewerModel::parent(const QModelIndex& index) const {
  if (!index.isValid())
    return {};

  auto* child_item = static_cast<MapObjectViewerItem*>(index.internalPointer());
  MapObjectViewerItem* parent_item = child_item->parent_;

  return parent_item != root_item_.get()
    ? createIndex(parent_item->row(), 0, parent_item) : QModelIndex{};
}

s32 MapObjectViewerModel::rowCount(const QModelIndex& parent) const {
  if (parent.column() > 0)
    return 0;

  const MapObjectViewerItem* parent_item = parent.isValid()
    ? static_cast<const MapObjectViewerItem*>(parent.internalPointer())
    : root_item_.get();

  return parent_item->children_.size();
}

s32 MapObjectViewerModel::columnCount(const QModelIndex& parent) const {
  return 2;
}

QVariant MapObjectViewerModel::data(const QModelIndex& index, s32 role) const {
  if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::ForegroundRole))
    return {};

  const auto* item = static_cast<MapObjectViewerItem*>(index.internalPointer());

  if (role == Qt::ForegroundRole) {
    return item->data_color();
  }

  return item->data(index.column());
}

Qt::ItemFlags MapObjectViewerModel::flags(const QModelIndex& index) const {
  return index.isValid()
    ? QAbstractItemModel::flags(index) : Qt::ItemFlags(Qt::NoItemFlags);
}

QVariant MapObjectViewerModel::headerData(s32 section, Qt::Orientation orientation, s32 role) const {
  return orientation == Qt::Horizontal && role == Qt::DisplayRole
    ? root_item_->data(section) : QVariant{};
}
