#include "ObjectViewerItem.h"
#include "../../Memory/Memory.h"
#include <QJsonArray>
#include <QJsonObject>

ObjectViewerItem::ObjectViewerItem(const QJsonObject& json, ObjectViewerItem* parent_item, bool is_attribute)
  : parent_item_(parent_item), is_attribute_(is_attribute){
  if (parent_item == nullptr) { // header
    QJsonArray objects = json["objects"].toArray();
    name_ = "name";
    value_ = "value";

    for (auto object : objects) {
      auto* child = new ObjectViewerItem(object.toObject(), this);
      append_child(child);
    }
  } else {  // data
    if (json["address"].isUndefined()) {
      address_ = 0;
      value_ = json["value"].toString().toUInt(nullptr, 16);
    } else {
      address_ = json["address"].toString().toUInt(nullptr, 16);
    }
    if (!json["p_functions"].isUndefined()) {
      p_functions_ = json["p_functions"].toString().toInt(nullptr, 16);
    }

    const QString string_type = json["type"].toString();
    if (string_type == "pointer") {
      type_ = POINTER;
      type_size_ = 4;
    } else if (string_type == "s8") {
      type_ = S8;
      type_size_ = 1;
    } else if (string_type == "s16") {
      type_ = S16;
      type_size_ = 2;
    } else if (string_type == "s32") {
      type_ = S32;
      type_size_ = 4;
    } else if (string_type == "u8") {
      type_ = U8;
      type_size_ = 1;
    } else if (string_type == "u16") {
      type_ = U16;
      type_size_ = 2;
    } else if (string_type == "u32") {
      type_ = U32;
      type_size_ = 4;
    } else if (string_type == "float") {
      type_ = FLOAT;
      type_size_ = 4;
    } else if (string_type == "string") {
      type_ = STRING;
      type_size_ = 4;
    }

    if (json["name"].isObject()) {
      memory_name_ = new ObjectViewerItem(json["name"].toObject(), this, true);
      name_ = memory_name_->name_;
    } else {
      name_ = json["name"].toString();
    }
    
    if (!json["count"].isUndefined() && json["count"].isObject()) {
      memory_count_ = new ObjectViewerItem(json["count"].toObject(), this, true);
    }

    if (!json["pointer"].isUndefined() && json["pointer"].isObject()) {
      memory_pointer_ = new ObjectViewerItem(json["pointer"].toObject(), this, true);
    }

    if (type_ == POINTER && !json["offsets"].isUndefined()) {
      QJsonArray offsets = json["offsets"].toArray();
      for (auto offset : offsets) {
        QJsonObject offset_json = offset.toObject();
        if (offset_json["address"].isObject()) {
          QJsonObject address = offset_json["address"].toObject();
          for (s64 i = 0; i < address["maximum_count"].toInt(); i++) {
            offset_json["address"] = QString::number(i * type_size_, 16);
            auto* child = new ObjectViewerItem(offset_json, this);
            append_child(child);
          }
        } else {
          auto* child = new ObjectViewerItem(offset_json, this);
          append_child(child);
        }
      }
    }
  }
}

ObjectViewerItem::~ObjectViewerItem() {
  qDeleteAll(child_items_);
  delete memory_name_;
  delete memory_count_;
  delete memory_pointer_;
}

void ObjectViewerItem::append_child(ObjectViewerItem* child) {
  child_items_.append(child);
}

s64 ObjectViewerItem::child_count() const {
  const s64 child_count = child_items_.count();
  if (memory_count_ != nullptr) {
    const s64 memory_count = memory_count_->value_.toUInt();
    if (memory_count < child_count)
      return memory_count;
    if (1000 < memory_count)
      return 0;
  }
  return child_count;
}

s64 ObjectViewerItem::row() const {
  if (parent_item_)
    return parent_item_->child_items_.indexOf(const_cast<ObjectViewerItem*>(this));

  return 0;
}

void ObjectViewerItem::update() {
  if (parent_item_ == nullptr) {
    //address = address_;
    return;
  }

  // get current address
  u32 pointer;
  if (!is_attribute_ && parent_item_->memory_pointer_ != nullptr) {
    pointer = parent_item_->memory_pointer_->value_.toUInt();
  } else {
    pointer = parent_item_->value_.toUInt();
  }
  u32 address = pointer + address_;

  if (memory_name_ != nullptr) {
    memory_name_->update();
    name_ = memory_name_->value_.toString();
  }

  if (memory_count_ != nullptr) {
    memory_count_->update();
    name_ += " (" + QString::number(memory_count_->value_.toUInt()) + ")";
  }

  if (memory_pointer_ != nullptr) {
    memory_pointer_->update();
  }

  if (address < 0x80000000 || 0x81800000 <= address)
    return;

  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  if (p_functions_ != 0 && memory::read_u32(pointer) != p_functions_) {
    return;
  }

  switch (type_) {
  case POINTER:
    value_ = memory::read_u32(address);
    break;
  case U8:
    value_ = memory::read_u8(address);
    break;
  case U16:
    value_ = memory::read_u16(address);
    break;
  case U32:
    value_ = memory::read_u32(address);
    break;
  case S8:
    value_ = memory::read_s8(address);
    break;
  case S16:
    value_ = memory::read_s16(address);
    break;
  case S32:
    value_ = memory::read_s32(address);
    break;
  case FLOAT:
    value_ = memory::read_float(address);
    break;
  case STRING:
    address = memory::read_u32(address);
    value_ = memory::read_string(address, 38);
    break;
  }
}

void ObjectViewerItem::update_all() {
  update();
  for (s32 i = 0; i < child_count(); i++) {
    child_items_[i]->update_all();
  }

  //update();
  //QVector<ObjectViewerItem*> items;
  //items << this;
  //while (!items.isEmpty()) {
  //  items.last()->update();
  //  for (s32 i = 0; i < items.last()->child_count(); i++) {
  //    items << items.last()->child_items_[i];
  //  }
  //  items.pop_back();
  //}
}

//ObjectViewerItem* ObjectViewerItem::child(int row) {
//  if (row < 0 || row >= child_items_.size())
//    return nullptr;
//  return child_items_.at(row);
//}
//
//
//int ObjectViewerItem::column_count() const {
//  //return item_data_.count();
//  return 2;
//}
//
//QVariant ObjectViewerItem::data(int column) const {
//  if (column < 0 || column >= 3)
//    return QVariant();
//  return item_data_.at(column);
//}
//
//ObjectViewerItem* ObjectViewerItem::parent_item() {
//  return parent_item_;
//}
//
