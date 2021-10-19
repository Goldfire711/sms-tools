#pragma once

#include <QAbstractTableModel>
#include "ObjectParametersItem.h"

class ObjectParametersModel : public QAbstractTableModel {
Q_OBJECT

public:
  ObjectParametersModel(QVector<ObjectParametersItem> *items, QObject* parent);
  ~ObjectParametersModel() override;

  enum {
    COLUMN_OFFSET = 0,
    COLUMN_NAME,
    COLUMN_VALUE,
    COLUMN_TYPE,
    COLUMN_CLASS,
    COLUMN_COUNT
  };

  int columnCount(const QModelIndex& parent) const override;
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void update_list();

  QVector<ObjectParametersItem>* items_;
};
