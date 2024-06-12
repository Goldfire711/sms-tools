#pragma once

#include <QGroupBox>
#include <QStyledItemDelegate>
#include <QTableView>
#include <vector>
#include "Common/CommonTypes.h"
#include "SMS/RNGManipulator/RNGRadioButton.h"
#include "SMS/RNGManipulator/RNGViewer.h"

class NameKuriRNG {
public:
  NameKuriRNG();
  NameKuriRNG(u32 seed);
  void set_seed(u32 seed);
  void set_index(u32 index);
  void set_search_range(const u32 search_range) {
    search_range_ = search_range;
  }
  void search();
  static QVariant header_data(s32 column);
  QVariant data(s32 row, s32 column) const;

  static struct Settings {
    struct {
      float min = 0.0f;
      float max = 1.0f;
    } distance = { 600.f, 1000.f },
      angle = { 0.f, 360.f },
      body_scale = { 0.6f, 1.0f },
      march_speed = { 1.0f, 1.5f };
    float generate_prop = 0.2f;
  } settings_;

  struct Result {
    u32 seed;
    u32 index;
    bool has_7a5b_index;
    u32 index_7a;
    u32 index_5b;

    float distance;
    float angle;
    float body_scale;
    float march_speed;
  };
  std::vector<Result> results_;

  u32 search_range_ = 100000;
  double probability_inv_ = 1;
private:
  u32 seed_ = 0;
  u32 index_ = 0;
};

class NameKuriManipulatorDelegate : public QStyledItemDelegate {
  Q_OBJECT
public:
  NameKuriManipulatorDelegate(QObject* parent = nullptr);
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
};

class NameKuriManipulatorModel : public QAbstractTableModel {
  Q_OBJECT

public:
  NameKuriManipulatorModel(QObject* parent, NameKuriRNG* rng);

  int columnCount(const QModelIndex& parent) const override;
  int rowCount(const QModelIndex& parent) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  QVariant data(const QModelIndex& index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  void update_list();

  NameKuriRNG* rng_;
};

class NameKuriManipulator : public QWidget {
  Q_OBJECT

public:
  NameKuriManipulator(QWidget* parent = nullptr);

private:
  void search() const;
  void on_context_menu_requested(const QPoint& pos);
  void copy_selection() const;

  QPushButton* btn_search_;
  RNGRadioButton* rdb_rng_;
  RNGViewer* rng_viewer_;

  QLineEdit* txb_distance_from_;
  QLineEdit* txb_distance_to_;
  QLineEdit* txb_angle_from_;
  QLineEdit* txb_angle_to_;
  QLineEdit* txb_generate_prop_;
  QLineEdit* txb_body_scale_from_;
  QLineEdit* txb_body_scale_to_;
  QLineEdit* txb_march_speed_from_;
  QLineEdit* txb_march_speed_to_;

  QLineEdit* txb_search_range_;

  NameKuriManipulatorDelegate* delegate_;
  NameKuriManipulatorModel* model_;
  QTableView* tbl_;

  QLabel* lbl_bottom_;
  QLabel* lbl_probability_;

  NameKuriRNG* rng_;
};
