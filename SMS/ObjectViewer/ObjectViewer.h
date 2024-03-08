#pragma once

#include <QWidget>
#include <QMainWindow>
#include "ui_ObjectViewer.h"
#include "ObjectViewerModel.h"
#include "ObjectParameters.h"


class ObjectViewer : public QMainWindow {
  Q_OBJECT

public:
  ObjectViewer(QWidget* parent = Q_NULLPTR);
  ~ObjectViewer() override;
  void refresh();
  void show_context_menu(const QPoint& pos);
  void copy_as_dmw_format();
  void on_update();

protected:
  void closeEvent(QCloseEvent* event) override;
  void showEvent(QShowEvent* event) override;

private:
  Ui::ObjectViewer ui;

  ObjectViewerModel* model_;
  ObjectParameters* object_parameters_ = nullptr;

  void show_widget_object_parameters();
  void on_tree_object_clicked(const QModelIndex& index);
  //s64 number_ = 0;
  //s8 former_stage_ = -1;
  //s8 former_episode_ = -1;
  u32 prev_conductor_ = 0;
  s32 prev_count_ = 0;
};
