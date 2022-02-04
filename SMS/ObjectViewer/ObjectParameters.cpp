#include "ObjectParameters.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>
#include <QInputDialog>
#include <fstream>
#include <filesystem>

#include "../../Memory/Memory.h"

using namespace memory;
using json = nlohmann::json;
json g_json_parameters;
extern QTimer* g_timer_100ms;

ObjectParameters::ObjectParameters(QWidget* parent)
  : QDockWidget(parent) {
  ui.setupUi(this);

  // read object parameters from ObjectParameters/*.json
  read_parameters();

  model_ = new ObjectParametersModel(&items_, this);
  ui.table_parameters->setModel(model_);
  //ui.table_parameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

  connect(g_timer_100ms, &QTimer::timeout, this, QOverload<>::of(&ObjectParameters::refresh_items));
  connect(ui.button_edit_parameters, &QPushButton::clicked, this, &ObjectParameters::show_edit_parameters_dialog);
  connect(ui.table_parameters, &QTableView::doubleClicked, this, &ObjectParameters::on_table_double_clicked);
}

ObjectParameters::~ObjectParameters() {
}

void ObjectParameters::set_label(QString string) {
  ui.label->setText(string);
}

void ObjectParameters::show_parameters(u32 address, s64 index) {
  address_ = address;
  // vtable, nameを取得(+0x0, +0x4)
  u32 vtable = read_u32(address);
  QString name = read_string(address + 0x4, { 0 }, 100);
  if (index != -1)
    name = QString::number(index) + ": " + name;

  // vtableの値からclass名をロード(json)
  QFile file("SMS/Resources/vtable_to_class_JP.json");
  if (!file.open(QIODevice::ReadOnly))
    return;
  const QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  const QJsonObject vtable_to_class = load_doc["Class"].toObject();
  const QString class_name = vtable_to_class[QString::number(vtable, 16)].toString();
  name += " <" + class_name + "> (0x" + QString::number(address, 16).toUpper() + ") ";

  // class名からoffsets(offset,type,nameのarray)をロード なければ_defaultをロード
  items_.clear();
  if (g_json_parameters[class_name.toStdString()].is_null()) {
    name += "_default.json";
    load_items_from_json(g_json_parameters["_default"], "_default");
  } else {
    QString filename = class_name + ".json";
    filename.replace("::", " ");
    filename.replace('<', '(');
    filename.replace('>', ')');
    name += filename;
    load_items_from_json(g_json_parameters[class_name.toStdString()], class_name);
  }
  ui.label->setText(name);
  refresh_items();
  ui.table_parameters->resizeColumnsToContents();
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

    QString type = QString::fromStdString(parameter["type"]);
    if (type_list.contains(type)) {
      items_.append(ObjectParametersItem(address_, parameter, class_name, base_offset, name));
    }
    else if (type.right(1) == "*") {
      items_.append(ObjectParametersItem(address_, parameter, class_name, base_offset, name, true));
    }
    else {
      if (g_json_parameters[type.toStdString()].is_null())
        continue;
      std::string str_offset = parameter["offset"];
      load_items_from_json(g_json_parameters[type.toStdString()], type, base_offset + std::stoi(str_offset, nullptr, 16), name);
    }
  }
}

void ObjectParameters::refresh_items() {
  for (s64 i = 0; i < items_.size(); i++) {
    items_[i].read_memory();
  }
  model_->update_list();
}

void ObjectParameters::show_edit_parameters_dialog() {
  //auto* edit_parameters = new EditParametersDialog(g_json_classes, this);
  //edit_parameters->exec();
  //delete edit_parameters;
  //EditParametersDialog edit_parameters(g_json_classes, this);
  //edit_parameters.exec();
}

// read object parameters from ObjectParameters/*.json
void ObjectParameters::read_parameters() {
  g_json_parameters = { {"dummy", 0} };
  for (const auto & entry : std::filesystem::directory_iterator("SMS/Resources/ObjectParameters")) {
    std::ifstream i(entry.path());
    json j;
    i >> j;
    g_json_parameters.insert(j.begin(), j.end());
  }
}

void ObjectParameters::on_table_double_clicked(const QModelIndex& index) {
  if (index == QVariant() || index.column() != ObjectParametersModel::COLUMN_VALUE)
    return;
  auto item = &items_[index.row()];
  bool ok = true;
  bool is_valid_value = false;
  QString string_invalid = "";
  while(ok && !is_valid_value) {
    QString message = "Type: " + item->string_type_ + string_invalid;
    QString string_value = QInputDialog::getText(this, tr("Change Value"), message, QLineEdit::Normal, item->string_value_, &ok);
    is_valid_value = item->write_memory_from_string(string_value);
    if (!is_valid_value)
      string_invalid = "\nInvalid value was entered";
  }
}
