#include "MapObjectParameters.h"

extern nlohmann::json g_vtable_to_class;

MapObjectParameters::MapObjectParameters(QWidget* parent)
  : QWidget(parent) {
  //const u32 p_mario = read_u32(0x8040a378);
  //params_list_.push_back(std::make_unique<ParamsBase>(p_mario));
  //parentWidget()->setWindowTitle("TNameKuri");
  //params_list_.push_back(std::make_unique<ParamsObjBase>(0x81638a14));
  //params_list_.push_back(std::make_unique<ParamsObjBase>(0x8163beb8));
  tbl_parameters_ = new QTableView();
  model_ = new MapObjectParametersModel(params_list_);

  tbl_parameters_->setModel(model_);

  auto* lo_main = new QHBoxLayout();
  lo_main->addWidget(tbl_parameters_);
  setLayout(lo_main);

  startTimer(33);
}

void MapObjectParameters::show_parameters(const u32 address) {
  const u32 vt = read_u32(address);
  std::stringstream ss;
  ss << std::hex << vt;
  //if (!g_vtable_to_class.contains(ss.str()))
  //  return;
  params_list_.clear();
  const QString class_name = QString::fromStdString(g_vtable_to_class[ss.str()]);
  parentWidget()->setWindowTitle(class_name);

  if (class_name == "TMario") {
    params_list_.push_back(std::make_unique<ParamsMario>(address));
  } else if (class_name == "CPolarSubCamera") {
    params_list_.push_back(std::make_unique<ParamsBase>(address));
  } else if (class_name.contains("Manager")) {
    params_list_.push_back(std::make_unique<ParamsManagerBase>(address));
  } else {
    const u32 p_manager = read_u32(address + 0x70);
    const s32 count = read_s32(p_manager + 0x14);
    const u32 p_obj_list = read_u32(p_manager + 0x18);
    for (s32 i = 0; i < count; i++) {
      const u32 p_obj = read_u32(p_obj_list + i * 4);
      const u32 vt_obj = read_u32(p_obj);
      std::stringstream ss2;
      ss2 << std::hex << vt_obj;
      if (g_vtable_to_class[ss2.str()] == class_name.toStdString()) {
        if (class_name == "TNameKuri")
          params_list_.push_back(std::make_unique<ParamsTNameKuri>(p_obj));
        else if (class_name == "TBossManta")
          params_list_.push_back(std::make_unique<ParamsTBossManta>(p_obj));
        else
          params_list_.push_back(std::make_unique<ParamsObjBase>(p_obj));
      }
    }
  }
  emit model_->layoutChanged();
  tbl_parameters_->resizeColumnsToContents();
}

void MapObjectParameters::append_params(const u32 ptr, const QString &class_name) {
  
}

void MapObjectParameters::timerEvent(QTimerEvent* event) {
  emit model_->layoutChanged();
}
