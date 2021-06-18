#include "ObjectViewer.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

extern QTimer* g_timer;

ObjectViewer::ObjectViewer(QWidget* parent)
  : QWidget(parent) {
  ui.setupUi(this);

  const QString file_path = ":/sms/ObjectViewer.json";
  QFile file(file_path);
  if (!file.open(QIODevice::ReadOnly)) {
    ui.label->setText("Couldn't open " + file_path);
    return;
  }
  QJsonDocument load_doc(QJsonDocument::fromJson(file.readAll()));
  file.close();
  model_ = new ObjectViewerModel(load_doc.object());

  ui.tree_object->setModel(model_);

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
