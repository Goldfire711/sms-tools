#pragma once

#include <QVariant>
#include <QVector>
#include <QString>
#include "../../Common/CommonTypes.h"

class ObjectViewerItem {
public:
  explicit ObjectViewerItem(const QJsonObject& json, ObjectViewerItem* parent_item = nullptr);
  ~ObjectViewerItem();

  void append_child(ObjectViewerItem* child);
  s64 row() const;
  s64 child_count() const;
  void update_all();

  enum {
    POINTER,
    S8,
    S16,
    S32,
    U8,
    U16,
    U32,
    FLOAT,
    STRING
  } type_;
  QString name_;
  QVariant value_ = 0;
  s32 column_count_ = 2;
  bool is_visible_ = true;

  QVector<ObjectViewerItem*> child_items_;
  ObjectViewerItem* parent_item_;
private:
  //QVector<QVariant> item_data_;
  s64 type_size_ = 4;
  u32 address_ = 0;

  ObjectViewerItem* memory_name_ = nullptr;
  ObjectViewerItem* memory_count_ = nullptr;

  void update();

};
