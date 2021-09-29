#pragma once

#include <QAbstractTableModel>

#include "FluffRNGFunctions.h"

class FluffManipulatorModel : public QAbstractTableModel
{
  Q_OBJECT

public:
  enum
  {
    FLUFF_COL_DIFFINDEX = 0,
    FLUFF_COL_7A5BDIFFINDEX,
    FLUFF_COL_X,
    FLUFF_COL_Y,
    FLUFF_COL_Z,
    FLUFF_COL_SEED,
    FLUFF_COL_NUM
  };

  FluffManipulatorModel(QObject* parent, std::vector<fluff_rng_functions::FluffResult>* fluffs_result);
  ~FluffManipulatorModel();

  int columnCount(const QModelIndex& parent) const override;
  int rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  void updateList();

private:
  std::vector<fluff_rng_functions::FluffResult>* m_fluffs_result_;
};
