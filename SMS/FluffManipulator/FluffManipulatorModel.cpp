#include "FluffManipulatorModel.h"

FluffManipulatorModel::FluffManipulatorModel(
  QObject* parent, std::vector<fluff_rng_functions::FluffResult>* fluffs_result)
  : QAbstractTableModel(parent), m_fluffs_result_(fluffs_result)
{
}

FluffManipulatorModel::~FluffManipulatorModel()
{
}

int FluffManipulatorModel::columnCount(const QModelIndex& parent) const
{
  return FLUFF_COL_NUM;
}

int FluffManipulatorModel::rowCount(const QModelIndex& parent) const
{
  return static_cast<int>(m_fluffs_result_->size());
}

QVariant FluffManipulatorModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole)
  {
    std::string string_7a5b;
    float x, y, z;
    switch (index.column())
    {
    case FLUFF_COL_DIFFINDEX:
      return m_fluffs_result_->at(index.row()).index;
    case FLUFF_COL_7A5BDIFFINDEX:
      if (m_fluffs_result_->at(index.row()).has_7a5b_index)
      {
        string_7a5b.append("(");
        string_7a5b.append(std::to_string(m_fluffs_result_->at(index.row()).index_7a5b.a));
        string_7a5b.append(" ");
        string_7a5b.append(std::to_string(m_fluffs_result_->at(index.row()).index_7a5b.b));
        string_7a5b.append(")");
      }
      return QString::fromStdString(string_7a5b);
    case FLUFF_COL_X:
      x = m_fluffs_result_->at(index.row()).x_from_target;
      return x;
    case FLUFF_COL_Y:
      y = m_fluffs_result_->at(index.row()).y_from_target;
      return y;
    case FLUFF_COL_Z:
      z = m_fluffs_result_->at(index.row()).z_from_target;
      return z;
    case FLUFF_COL_SEED:
      return QString::number(m_fluffs_result_->at(index.row()).rng_seed, 16).toUpper();
    }
  }
  return QVariant();
}

QVariant FluffManipulatorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
  {
    switch (section)
    {
    case FLUFF_COL_DIFFINDEX:
      return tr("Index+");
    case FLUFF_COL_7A5BDIFFINDEX:
      return tr("=7a+5b");
    case FLUFF_COL_X:
      return tr("X - Target");
    case FLUFF_COL_Y:
      return tr("Y - Target");
    case FLUFF_COL_Z:
      return tr("Z - Target");
    case FLUFF_COL_SEED:
      return ("RNG Seed");
    }
  }
  return QVariant();
}

void FluffManipulatorModel::updateList()
{
  emit layoutChanged();
}