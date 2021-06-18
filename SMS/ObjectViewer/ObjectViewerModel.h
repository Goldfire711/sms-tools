#pragma once

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "../../Common/CommonTypes.h"

class ObjectViewerItem;

class ObjectViewerModel : public QAbstractItemModel {
Q_OBJECT

public:
  explicit ObjectViewerModel(const QJsonObject &json, QObject* parent = nullptr);
  ~ObjectViewerModel() override;

  QVariant data(const QModelIndex& index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column,
                    const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  int columnCount(const QModelIndex& parent = QModelIndex()) const override;

  void on_update();

  s64 time_count_ = 0;

private:
  enum {
    COLUMN_NAME,
    COLUMN_VALUE
  };
  //void setup_model_data(const QJsonObject &json, ObjectViewerItem* parent);

  ObjectViewerItem* root_item_;
};
