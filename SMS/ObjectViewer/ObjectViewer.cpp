#include "ObjectViewer.h"
#include "../../DolphinProcess/DolphinAccessor.h"
#include "../../Common/CommonUtils.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QJsonArray>
#include <QTextCodec>

extern QTimer* g_timer;

ObjectViewer::ObjectViewer(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);

  const QString file_path = "SMS/Resources/ObjectViewer.json";
  QFile file(file_path);
  if (!file.open(QIODevice::ReadOnly)) {
    ui.label->setText("Couldn't open " + file_path);
    return;
  } else {
    ui.label->setText(file_path);
  }
  QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  model_ = new ObjectViewerModel(load_doc.object());

  ui.tree_object->setModel(model_);

  connect(ui.button_scan_managers, &QPushButton::clicked, this, &ObjectViewer::scan_managers);
  connect(g_timer, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
}

ObjectViewer::~ObjectViewer() {
  delete model_;
}

void ObjectViewer::closeEvent(QCloseEvent* event) {
  disconnect(g_timer, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
}

void ObjectViewer::showEvent(QShowEvent* event) {
  connect(g_timer, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
}

void ObjectViewer::scan_managers() {
  if (DolphinComm::DolphinAccessor::getStatus() != DolphinComm::DolphinAccessor::DolphinStatus::hooked)
    return;

  // delete model_
  if (model_ != nullptr) {
    disconnect(g_timer, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
    ui.tree_object->setModel(nullptr);
    delete model_;
    model_ = nullptr;
  }

  // read ObjectViewer.json
  const QString file_path = "SMS/Resources/ObjectViewer.json";
  QFile file3(file_path);
  if (!file3.open(QIODevice::ReadOnly)) {
    ui.label->setText("Couldn't open " + file_path);
    return;
  }
  else {
    ui.label->setText(file_path);
  }
  const QJsonDocument load_doc3(QJsonDocument::fromJson(file3.readAll()));
  file3.close();
  QJsonObject json = load_doc3.object();
  QJsonArray json_array = json["objects"].toArray();

  // read entire memory
  constexpr u32 mem_start = 0x80414020;
  constexpr u32 mem_end = 0x81800000;
  constexpr u32 mem_size = mem_end - mem_start;
  u8* scan_ram_cache = new u8[mem_size];
  if (!DolphinComm::DolphinAccessor::readFromRAM(mem_start & 0x7fffffff, scan_ram_cache, mem_size, false)) {
    delete[] scan_ram_cache;
    return;
  }

  // read name,address from ObjectViewer_managers.json
  QFile file("SMS/Resources/ObjectViewer_managers.json");
  if (!file.open(QIODevice::ReadOnly))
    return;
  const QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  QJsonArray json_managers = load_doc["managers"].toArray();
  QJsonObject json_template = load_doc["object_template"].toObject();

  // read template from json
  //QFile file2("SMS/Resources/ObjectViewer_template.json");
  //if (!file2.open(QIODevice::ReadOnly))
  //  return;
  //const QJsonDocument load_doc2(QJsonDocument::fromJson(file2.readAll()));
  //file2.close();
  //QJsonArray json_templates = load_doc2.object()["objects"].toArray();
  //QJsonObject json_template = json_templates.at(0).toObject();

  // scan managers
  for (auto manager : json_managers) {
    const QString name = manager.toObject()["name"].toString();
    QByteArray ba = QTextCodec::codecForName("Shift-JIS")->fromUnicode(name);
    ba.append('\0');
    const u32 p_functions_bswap = Common::bSwap32(manager.toObject()["p_functions"].toString().toUInt(nullptr, 16));
    const s64 p_functions_offset = (ba.size() + 3) & 0b11111100; // == (ba.size()+4) - (ba.size()%4)

    for (u32 i = 0; i < (mem_size - 0x100); i += 4) {
      u8* memory_candidate = &scan_ram_cache[i];
      if (std::memcmp(memory_candidate, ba.data(), ba.size()) == 0 &&
        std::memcmp(&memory_candidate[p_functions_offset], &p_functions_bswap, 4) == 0) {
        json_template["value"] = QString::number(i + p_functions_offset + mem_start, 16);
        json_template["name"].toObject()["name"] = name;
        //json_template["name"] = name;
        json_array.append(json_template);
      }
    }
  }

  delete[] scan_ram_cache;

  json.insert("objects", json_array);

  // setup model_
  model_ = new ObjectViewerModel(json);
  ui.tree_object->setModel(model_);
  connect(g_timer, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
}

void ObjectViewer::test() {
  const QString file_path = "SMS/Resources/ObjectViewer.json";
  QFile file(file_path);
  if (!file.open(QIODevice::ReadOnly)) {
    ui.label->setText("Couldn't open " + file_path);
    return;
  }
  else {
    ui.label->setText(file_path);
  }
  QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  model_ = new ObjectViewerModel(load_doc.object());
  ui.tree_object->setModel(model_);
  connect(g_timer, &QTimer::timeout, model_, QOverload<>::of(&ObjectViewerModel::on_update));
}


//void ObjectViewer::test() {
//  //QFile loadFile(":/sms/ObjectViewer.json");
//  //if (!loadFile.open(QIODevice::ReadOnly)) {
//  //  ui.label->setText("failed");
//  //  return;
//  //}
//  //QByteArray bytes = loadFile.readAll();
//  //loadFile.close();
//  //QJsonDocument loadDoc(QJsonDocument::fromJson(bytes));
//  //QJsonObject json = loadDoc.object();
//  //QJsonObject json2 = json["objects"].toObject();
//  //ui.label->setText(json2["address"].toString());
//
//  QFile loadFile(":/sms/test.json");
//  if (!loadFile.open(QIODevice::ReadOnly)) {
//    ui.label->setText("Couldn't open :/sms/test.json");
//    return;
//  }
//  QByteArray bytes = loadFile.readAll();
//  loadFile.close();
//  QJsonDocument loadDoc(QJsonDocument::fromJson(bytes));
//  QJsonObject json = loadDoc.object();
//  ui.label->setText(loadDoc["test1"]["test2"].toString());
//
//}

//void ObjectViewer::on_update() {
//  ui.label->setText(QString::number(number_++));
//}
