#include <fstream>
#include "ObjectViewerModel.h"
#include "ObjectViewerItem.h"
#include "../../Memory/Memory.h"
#include <QColor>
#include <QDebug>

ObjectViewerModel::ObjectViewerModel(const QJsonObject &json, QObject* parent)
  : QAbstractItemModel(parent) {
  root_item_ = new ObjectViewerItem(json);
  //setup_model_data(json, root_item_);
}

ObjectViewerModel::~ObjectViewerModel() {
  delete root_item_;
}

int ObjectViewerModel::columnCount(const QModelIndex& parent) const
{
  return COLUMN_COUNT;
}

QVariant ObjectViewerModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole && role != Qt::ForegroundRole)
    return QVariant();

  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return QVariant();

  //float mem = memory::read_float(0x8040A378, {0xb0});
  auto* item = static_cast<ObjectViewerItem*>(index.internalPointer());
  //if (item->is_attribute_)
  //  return QVariant();

  if (role == Qt::ForegroundRole) {
    if (item->parent_item_ == root_item_)
      return {};
    const u32 draw_info = memory::read_u32(item->value_.toUInt() + 0xf0);
    if (draw_info & 1)  // isInactive
      return QColor(Qt::gray);
    if (!(draw_info & 4)) // isInCamera
      return QColor(Qt::blue);
    return {};
  }

  switch (index.column()) {
  case COLUMN_NAME:
    return item->name_;
  case COLUMN_CLASS:
    return item->class_name_;
  case COLUMN_VALUE:
    if (item->type_ == item->POINTER)
      return "0x" + QString::number(item->value_.toUInt(), 16).toUpper();
    return item->value_;
  default:
    return QVariant();
  }

  //return item->data(index.column());
}

Qt::ItemFlags ObjectViewerModel::flags(const QModelIndex& index) const
{
  if (!index.isValid())
    return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index);
}

QVariant ObjectViewerModel::headerData(int section, Qt::Orientation orientation,
  int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    //return root_item_->data(section);
    switch (section) {
    case COLUMN_NAME:
      return root_item_->name_;
    case COLUMN_CLASS:
      return root_item_->class_name_;
    case COLUMN_VALUE:
      return root_item_->value_;
    }
  }

  return QVariant();
}

QModelIndex ObjectViewerModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  ObjectViewerItem* parentItem;

  if (!parent.isValid())
    parentItem = root_item_;
  else
    parentItem = static_cast<ObjectViewerItem*>(parent.internalPointer());

  ObjectViewerItem* childItem = parentItem->child_items_[row];
  if (childItem)
    return createIndex(row, column, childItem);
  return QModelIndex();
}

QModelIndex ObjectViewerModel::parent(const QModelIndex& index) const
{
  if (!index.isValid())
    return QModelIndex();

  ObjectViewerItem* childItem = static_cast<ObjectViewerItem*>(index.internalPointer());
  ObjectViewerItem* parentItem = childItem->parent_item_;

  if (parentItem == root_item_)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int ObjectViewerModel::rowCount(const QModelIndex& parent) const
{
  ObjectViewerItem* parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = root_item_;
  else
    parentItem = static_cast<ObjectViewerItem*>(parent.internalPointer());

  return parentItem->child_count();
}

void ObjectViewerModel::update_all_items(ObjectViewerItem* item, const QModelIndex& parent) {
  item->update();
  emit dataChanged(index(item->row(), COLUMN_NAME, parent), index(item->row(), COLUMN_VALUE, parent));
  for (s32 i = 0; i < item->child_count(); i++) {
    item->child_items_[i]->update_all();
  }
}

void ObjectViewerModel::on_update() {
  //time_count_++;
  //root_item_->update_all();
  //emit layoutChanged();
  update_all_items(root_item_);
}

ObjectViewerItem* ObjectViewerModel::get_item(const QModelIndex& index) const {
  return static_cast<ObjectViewerItem*>(index.internalPointer());
}