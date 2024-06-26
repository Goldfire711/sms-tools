#include "MapObjectParameters.h"

extern nlohmann::json g_vtable_to_class;

MapObjectParameters::MapObjectParameters(QWidget* parent)
  : QDockWidget(parent) {
  setWindowTitle("Watcher");

  tbl_parameters_ = new QTableView();

  model_ = new MapObjectParametersModel(params_list_);
  tbl_parameters_->setModel(model_);

  auto font = tbl_parameters_->font();
  font.setPointSize(9);
  tbl_parameters_->setFont(font);

  tbl_parameters_->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(tbl_parameters_, &QAbstractItemView::clicked, this, &MapObjectParameters::on_table_clicked);
  connect(tbl_parameters_, &QAbstractItemView::customContextMenuRequested, this, &MapObjectParameters::on_table_right_clicked);

  setWidget(tbl_parameters_);

  setHidden(!Settings::instance().IsMapObjectWatcherVisible());
  connect(&Settings::instance(), &Settings::MapObjectWatcherVisibilityChanged, this,
    [this](const bool visible) {setHidden(!visible); });

  startTimer(33);
}

MapObjectParameters::~MapObjectParameters() = default;

void MapObjectParameters::closeEvent(QCloseEvent* event) {
  Settings::instance().SetMapObjectWatcherVisible(false);
}

void MapObjectParameters::show_parameters(const u32 address) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;
  const u32 vt = read_u32(address);
  if (vt < 0x80000000 || 0x817fffff < vt)
    return;
  std::stringstream ss;
  ss << std::hex << vt;
  if (!g_vtable_to_class.contains(ss.str()))
    return;
  params_list_.clear();
  const QString class_name = QString::fromStdString(g_vtable_to_class[ss.str()]);
  setWindowTitle(class_name);

  model_->selected_column_ = -1;
  if (class_name == "TMario") {
    params_list_.push_back(std::make_unique<ParamsMario>(address));
  } else if (class_name == "CPolarSubCamera") {
    params_list_.push_back(std::make_unique<ParamsBase>(address));
  } else if (class_name.contains("Manager")) {
    params_list_.push_back(std::make_unique<ParamsManagerBase>(address));
  } else if (class_name == "TBGTakeHit") {
    const u32 p_bg = read_u32(address + 0x70, { 0x34 });
    for (s32 i = 0; i < 4; i++) {
      const u32 p_leg = read_u32(p_bg + 0x150 + i * 4, { 0x3c });
      params_list_.push_back(std::make_unique<ParamsObjBase>(p_leg));
    }
  } else if (class_name == "TBGBeakHit") {
    params_list_.push_back(std::make_unique<ParamsObjBase>(address));
  } else {
    const u32 p_manager = read_u32(address + 0x70);
    const s32 count = read_s32(p_manager + 0x14);
    if (count < 0 || 300 < count)
      return;
    const u32 p_obj_list = read_u32(p_manager + 0x18);
    for (s32 i = 0; i < count; i++) {
      const u32 p_obj = read_u32(p_obj_list + i * 4);
      //std::stringstream ss2;
      //ss2 << std::hex << vt_obj;
      //if (g_vtable_to_class[ss2.str()] == class_name.toStdString()) {
      if (const u32 vt_obj = read_u32(p_obj); vt_obj == vt) {
        if (class_name == "TNameKuri")
          params_list_.push_back(std::make_unique<ParamsTNameKuri>(p_obj));
        else if (class_name == "TBossManta")
          params_list_.push_back(std::make_unique<ParamsTBossManta>(p_obj));
        else if (class_name == "TBossGesso")
          params_list_.push_back(std::make_unique<ParamsTBossGesso>(p_obj));
        else
          params_list_.push_back(std::make_unique<ParamsObjBase>(p_obj));

        if (p_obj == address)
          model_->selected_column_ = static_cast<s32>(params_list_.size()) - 1;
      }
    }
  }
  emit model_->layoutChanged();
  tbl_parameters_->resizeColumnsToContents();
  tbl_parameters_->resizeRowsToContents();

  // scroll to selected
  const auto index = model_->index(0, model_->selected_column_);
  const s32 vertical_scroll = tbl_parameters_->verticalScrollBar()->value();
  tbl_parameters_->scrollTo(index, QAbstractItemView::PositionAtCenter);
  tbl_parameters_->verticalScrollBar()->setValue(vertical_scroll);
}

void MapObjectParameters::timerEvent(QTimerEvent* event) {
  emit model_->layoutChanged();
}

void MapObjectParameters::on_table_clicked(const QModelIndex& index) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  model_->selected_column_ = index.column();

  auto* item = static_cast<ParamsBase*>(index.internalPointer());
  emit item_clicked(item->ptr_);
}

void MapObjectParameters::on_table_right_clicked(const QPoint& pos) {
  if (DolphinComm::DolphinAccessor::getStatus() !=
    DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  const auto index = tbl_parameters_->indexAt(pos);

  model_->selected_column_ = index.column();

  const auto* item = static_cast<ParamsBase*>(index.internalPointer());
  if (item != nullptr) {
    emit item_clicked(item->ptr_);
    emit item_right_clicked(item->ptr_);
  }
}
