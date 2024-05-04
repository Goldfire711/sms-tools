#pragma once
#include <QAbstractItemModel>
#include <QWidget>
#include <QTreeView>
#include <QHBoxLayout>

#include <json.hpp>
#include "Common/CommonTypes.h"
#include "Memory/Memory.h"

using namespace memory;
extern nlohmann::json g_vtable_to_class;

class MapObjectViewerItem {
public:
  explicit MapObjectViewerItem();
  explicit MapObjectViewerItem(u32 p_obj, MapObjectViewerItem* parent = nullptr);
  ~MapObjectViewerItem();

  void append_manager(u32 p_manager);
  MapObjectViewerItem* append_object(u32 p_obj);
  QVariant data(s32 column) const;
  s32 row() const;
  QColor data_color() const;

  QString name_;
  QString class_name_;
  u32 address_ = 0;
  bool is_manager_ = false;

  MapObjectViewerItem* parent_ = nullptr;
  std::vector<std::unique_ptr<MapObjectViewerItem>> children_;
};

class MapObjectViewerModel : public QAbstractItemModel {
  Q_OBJECT

public:
  Q_DISABLE_COPY_MOVE(MapObjectViewerModel);

  explicit MapObjectViewerModel(QObject* parent = nullptr);
  ~MapObjectViewerModel() override;

  QVariant data(const QModelIndex& index, s32 role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(s32 section, Qt::Orientation orientation,
    s32 role = Qt::DisplayRole) const override;
  QModelIndex index(s32 row, s32 column, const QModelIndex& parent = {}) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  s32 rowCount(const QModelIndex& parent = {}) const override;
  s32 columnCount(const QModelIndex& parent = {}) const override;

  std::unique_ptr<MapObjectViewerItem> root_item_;
};

class MapObjectViewer : public QWidget {
  Q_OBJECT

public:
  MapObjectViewer(QWidget* parent = Q_NULLPTR);
  void refresh();
  void set_timer_interval(s32 interval);

protected:
  void timerEvent(QTimerEvent* event) override;

private:
  s32 timer_id_ = 0;
  s32 timer_interval_ = 33;
  MapObjectViewerModel* model_ = nullptr;
  QTreeView* tree_view_;
};
