#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_MainWindow.h"
#include "SMS/Spin/Spin.h"
#include "SMS/ObjectViewer/ObjectViewer.h"
#include "SMS/MapViewer/MapViewer.h"
#include "SMS/MapViewer/MapPinnaBeach.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	void closeEvent(QCloseEvent* event) override;
	void hook_unhook_attempt();

	void show_widget_spin();
  void show_widget_rng_manipulator();
	void show_widget_map_viewer();
	void show_widget_object_viewer();
	void on_update();

private:
	Ui::MainWindowClass ui;

	Spin* sms_spin_ = nullptr;
	ObjectViewer* sms_object_viewer_ = nullptr;
	MapViewer* sms_map_viewer = nullptr;
};