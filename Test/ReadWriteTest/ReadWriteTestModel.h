#pragma once

#include <QAbstractTableModel>
#include "ReadWriteTestItem.h"

class ReadWriteTestModel : public QAbstractTableModel {
Q_OBJECT

public:
  ReadWriteTestModel(QVector<ReadWriteTestItem>* items, QObject* parent = nullptr);
  ~ReadWriteTestModel() override;

  enum {
    COLUMN_NAME = 0,
    COLUMN_TYPE,
    COLUMN_ADDRESS,
    COLUMN_VALUE,
    COLUMN_COUNT
  };

  int columnCount(const QModelIndex& parent) const override;
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void update_list();

  QVector<ReadWriteTestItem>* items_;

private:
};
