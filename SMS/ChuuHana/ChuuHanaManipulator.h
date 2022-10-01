#pragma once

#include <QWidget>
#include "ChuuHanaRNG.h"
#include "ChuuHanaManipulatorModel.h"
#include "RNGViewer.h"

#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTableView>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QGroupBox>

#include "Common/CommonTypes.h"

class ChuuHanaManipulator : public QWidget {
  Q_OBJECT

public:
  ChuuHanaManipulator(QWidget* parent = nullptr);
  enum RadioButtonRNG {
    READ_FROM_RAM = 0,
    EDIT_RNGSEED,
    EDIT_RNGINDEX
  };

  enum RadioButtonType {
    TYPE_AUTO = 0,
    TYPE_SET_GOAL,
    TYPE_NODE
  };

  enum ComboBoxNodeRange {
    ZERO_TO_SIX = 0,
    ZERO_TO_SEVEN
  };

private:
  void initialize_widgets();
  void make_layouts();
  void update();
  void update_rng_textbox();
  void on_rng_seed_changed();
  void on_rng_index_changed();
  void on_node_count_changed(s32 index);
  void on_chb_node_multiple_changed(s32 state);
  void on_rng_type_changed(s32 id);
  void on_context_menu_requested(const QPoint& pos);
  void copy_selection() const;
  QLabel* lbl_rng_seed_;
  QLabel* lbl_rng_index_;
  QRadioButton* rdb_read_from_ram_;
  QRadioButton* rdb_edit_rng_seed_;
  QRadioButton* rdb_edit_rng_index_;
  QButtonGroup* group_rdb_rng_;
  QLineEdit* txb_rng_seed_;
  QLineEdit* txb_rng_index_;
  QPushButton* btn_search_;
  ChuuHanaManipulatorModel* model_tbl_rng_;;
  QTableView* tbl_rng_;
  QLineEdit* txb_range_from_;
  QLineEdit* txb_range_to_;
  QLineEdit* txb_search_range_;
  QLabel* lbl_elapsed_time_;
  QLabel* lbl_probability_;
  QLabel* lbl_nodes_;
  QButtonGroup* group_chb_nodes_;
  QRadioButton* rdb_type_auto_;
  QRadioButton* rdb_type_set_goal_;
  QRadioButton* rdb_type_node_;
  QButtonGroup* group_rdb_rng_type_;
  QLabel* lbl_type_;
  QCheckBox* chb_node_collid_move_;
  QComboBox* cmb_node_range_;
  QCheckBox* chb_node_multiple_;
  QGroupBox* group_set_goal_;
  QGroupBox* group_node_;

  u32 ram_seed_ = 0;
  u32 ram_index_ = 0;
  u32 edited_seed_ = 0;
  u32 edited_index_ = 0;
  ChuuHanaRNG* rng_;

  RNGViewer* rng_viewer_ = nullptr;

  struct ChuuHanaRNGType {
    s32 id = -1;
    s32 rng_type = -1;
    bool is_collid_move = false;

    bool operator==(const ChuuHanaRNGType& c) const {
      if (id == c.id && rng_type == c.rng_type && is_collid_move == c.is_collid_move) {
        return true;
      }
      return false;
    }
  };
  std::vector<ChuuHanaRNGType> detected_types_;
  s32 select_search_ = 0;
  s32 select_ = 0;
  QPushButton* btn_auto_left_;
  QPushButton* btn_auto_right_;
  void on_button_left_clicked();
  void on_button_right_clicked();
};
