#include "ObjectParameters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>
#include <QInputDialog>
#include <QMessageBox>
#include <fstream>
#include <filesystem>
#include <QSettings>
#include <QDesktopServices>

#include "../../Memory/Memory.h"
#include "Settings/Settings.h"

using namespace memory;
using json = nlohmann::json;
extern QTimer* g_timer_100ms;
extern nlohmann::json g_vtable_to_class;

ObjectParameters::ObjectParameters(QWidget* parent)
  : QDockWidget(parent) {
  ui.setupUi(this);
  ui.button_open_folder->setIcon(style()->standardIcon((QStyle::SP_DirIcon)));
  ui.button_open_folder->setToolTip("Open json location");

  model_ = new ObjectParametersModel(&items_, this);
  ui.table_parameters->setModel(model_);

  auto font = ui.table_parameters->font();
  font.setPointSize(9);
  ui.table_parameters->setFont(font);

  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ObjectParameters::refresh_items));
  connect(ui.table_parameters, &QTableView::doubleClicked, this, &ObjectParameters::on_table_double_clicked);
  connect(ui.button_reload_json, &QPushButton::clicked, this, &ObjectParameters::reload_json);
  connect(ui.button_open_folder, &QPushButton::clicked, this, &ObjectParameters::open_json_location);

  auto& settings = Settings::GetQSettings();
  restoreGeometry(settings.value("object_parameters/geometry").toByteArray());
  setFloating(settings.value("object_parameters/floating").toBool());
}

ObjectParameters::~ObjectParameters() {
  auto& settings = Settings::GetQSettings();
  settings.setValue("object_parameters/geometry", saveGeometry());
  settings.setValue("object_parameters/floating", isFloating());
}

void ObjectParameters::show_parameters(u32 address, s64 index) {
  // read object parameters from ObjectParameters/*.json
  read_parameters();

  address_ = address;
  index_ = index;
  // vtable, nameを取得(+0x0, +0x4)
  u32 vtable = read_u32(address);
  QString info = read_string(address + 0x4, { 0 }, 100);
  if (index != -1)
    info = QString::number(index) + ": " + info;

  std::stringstream stream;
  stream << std::hex << vtable;
  if (!g_vtable_to_class.contains(stream.str()))
    return;
  const QString class_name = QString::fromStdString(g_vtable_to_class[stream.str()]);
  info += " <" + class_name + "> (0x" + QString::number(address, 16).toUpper() + ") ";

  // class名からoffsets(offset,type,nameのarray)をロード なければ_defaultをロード
  items_.clear();
  QString filename;
  if (json_parameters_[class_name.toStdString()].is_null()) {
    filename = "_default.json";
    load_items_from_json(json_parameters_["_default"], "_default");
  } else {
    filename = class_name + ".json";
    filename.replace("::", " ");
    filename.replace('<', '(');
    filename.replace('>', ')');
    load_items_from_json(json_parameters_[class_name.toStdString()], class_name);
  }
  ui.label->setText(info + filename);
  refresh_items();
  ui.table_parameters->resizeColumnsToContents();
  ui.table_parameters->resizeRowsToContents();
}

void ObjectParameters::load_items_from_json(const nlohmann::json& j, const QString& class_name, u32 base_offset, const QString& parent_name) {
  const QStringList type_list = { "u8", "u16", "u32", "s8", "s16", "s32", "float", "string" };
  for (auto& parameter : j["offsets"]) {
    QString name;
    if (parent_name.contains('*')) {
      name = parent_name;
      name.replace("*", QString::fromStdString(parameter["name"]));
    }
    else {
      name = QString::fromStdString(parameter["name"]);
    }
    // $(0xXX)があったときは0xXXから名前を読み込む
    QRegExp rx(R"(\$\((0x[0-9a-fA-F]+)\))");
    while (rx.indexIn(name) > -1) {
      const QString str_offset = rx.cap(1);
      const s32 offset = str_offset.toInt(nullptr, 16);
      QString mem_name;
      if (DolphinComm::DolphinAccessor::getStatus() == DolphinComm::DolphinAccessor::DolphinStatus::hooked) {
        mem_name = read_string(address_ + base_offset + offset, { 0 }, 50);
      }
      name.replace(name.indexOf(rx), str_offset.size() + 3, mem_name);
    }

    QString type = QString::fromStdString(parameter["type"]);
    if (type_list.contains(type)) {
      items_.append(ObjectParametersItem(address_, parameter, class_name, base_offset, name));
    }
    else if (type.right(1) == "*") {
      items_.append(ObjectParametersItem(address_, parameter, class_name, base_offset, name, true));
    }
    else {
      if (json_parameters_[type.toStdString()].is_null())
        continue;
      std::string str_offset = parameter["offset"];
      load_items_from_json(json_parameters_[type.toStdString()], type, base_offset + std::stoi(str_offset, nullptr, 16), name);
    }
  }
}

void ObjectParameters::refresh_items() {
  for (s64 i = 0; i < items_.size(); i++) {
    items_[i].read_memory();
  }
  model_->update_list();
}

// read object parameters from ObjectParameters/*.json
void ObjectParameters::read_parameters() {
  json_parameters_ = { {"dummy", 0} };
  for (const auto & entry : std::filesystem::directory_iterator("SMS/Resources/ObjectParameters")) {
    std::ifstream i(entry.path());
    json j;
    i >> j;
    json_parameters_.insert(j.begin(), j.end());
  }
}

void ObjectParameters::on_table_double_clicked(const QModelIndex& index) {
  if (index == QVariant())
    return;
  auto item = &items_[index.row()];

  if (index.column() == ObjectParametersModel::COLUMN_VALUE) {
    bool ok = true;
    bool is_valid_value = false;
    QString string_invalid = "";
    while (ok && !is_valid_value) {
      QString message = "Type: " + item->string_type_ + string_invalid;
      QString string_value = QInputDialog::getText(this, tr("Change Value"), message, QLineEdit::Normal, item->string_value_, &ok);
      if (ok) {
        is_valid_value = item->write_memory_from_string(string_value);
        if (!is_valid_value)
          string_invalid = "\nInvalid value was entered";
      }
    }
  } else if (index.column() == ObjectParametersModel::COLUMN_NOTES) {
    if (!item->notes_.isEmpty()) {
      QMessageBox show_notes(QMessageBox::NoIcon, "Notes", "", QMessageBox::Close);
      QString text = item->name_ + " (" + item->string_type_ + ")\n";
      text += "--------------------------------\n";
      text += item->notes_;
      show_notes.setText(text);

      show_notes.exec();
    }
  } else if (index.column() == ObjectParametersModel::COLUMN_TYPE) {
    if (item->string_type_.right(1) == '*') {
      if (!sub_parameters_)
        sub_parameters_ = new ObjectSubParameters(nullptr);
      sub_parameters_->show();
      sub_parameters_->raise();
      sub_parameters_->activateWindow();
      const u32 address = read_u32(item->address_);
      QString class_name = item->string_type_;
      class_name.chop(1);
        sub_parameters_->show_parameters(address, class_name, item->name_);

    }
  }
}

void ObjectParameters::reload_json() {
  show_parameters(address_, index_);
}

void ObjectParameters::open_json_location() {
  QDesktopServices::openUrl(QUrl::fromLocalFile("SMS/Resources/ObjectParameters"));
}
