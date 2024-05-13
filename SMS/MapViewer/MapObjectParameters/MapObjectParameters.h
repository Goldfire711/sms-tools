#pragma once
#include <QCheckBox>
#include <QDockWidget>
#include <QTableView>
#include <QWidget>
#include <QHBoxLayout>
#include <QTimer>
#include <json.hpp>
#include <QSortFilterProxyModel>
#include <QScrollBar>

#include "Params.h"
#include "Common/CommonTypes.h"

class MapObjectParametersModel : public QAbstractTableModel {
  Q_OBJECT

public:
  Q_DISABLE_COPY_MOVE(MapObjectParametersModel)

  explicit MapObjectParametersModel(const std::vector<std::unique_ptr<ParamsBase>> &params_list, QObject* parent = nullptr);
  //explicit MapObjectParametersModel(QObject* parent = nullptr);

  s32 columnCount(const QModelIndex& parent) const override;
  s32 rowCount(const QModelIndex& parent) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;

  s32 selected_column_ = 0;

private:
  const std::vector<std::unique_ptr<ParamsBase>> &params_list_;
};

class MapObjectParameters : public QWidget {
  Q_OBJECT

public:
  MapObjectParameters(QWidget* parent = Q_NULLPTR);

public slots:
  void show_parameters(u32 address);

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  QTableView* tbl_parameters_;
  std::vector<std::unique_ptr<ParamsBase>> params_list_;
  MapObjectParametersModel* model_;
};
