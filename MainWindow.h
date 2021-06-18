#pragma once

#include <QtWidgets/QMainWindow>
#include <QTimer>
#include "ui_MainWindow.h"
#include "SMS/Spin/Spin.h"
#include "SMS/ObjectViewer/ObjectViewer.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	void hook_unhook_attempt();

	void show_widget_spin();
  void show_widget_fluff_manipulator();
	void show_widget_map_viewer();
	void show_widget_object_viewer();
	void on_update();

private:
	Ui::MainWindowClass ui;

	s64 count_ = 0;
	bool update_ = false;
	Spin* sms_spin_ = nullptr;
	ObjectViewer* sms_object_viewer_ = nullptr;
};