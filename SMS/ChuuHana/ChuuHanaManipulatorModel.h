#pragma once

#include <QAbstractTableModel>

#include "ChuuHanaRNG.h"

class ChuuHanaManipulatorModel  : public QAbstractTableModel {
  Q_OBJECT

public:
  enum {
    COL_INDEX = 0,
    COL_7A5B_INDEX,
    COL_VALUE,
    COL_SEED,
    COL_COUNT
  };

  ChuuHanaManipulatorModel(QObject *parent, ChuuHanaRNG* rng);
  ~ChuuHanaManipulatorModel();

  int columnCount(const QModelIndex& parent) const override;
  int rowCount(const QModelIndex& parent) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  void update_list();

private:
  ChuuHanaRNG* rng_;
};
