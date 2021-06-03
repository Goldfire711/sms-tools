#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(QWidget* parent = Q_NULLPTR);
	void HookAttempt();
	void Unhook();
	void UpdateDolphinHookingStatus();
	void OnButtonHookClicked();

	std::string GetGameID();

	void ShowSpin();
  void ShowFluffManipulator();
	void ShowMapViewer();

private:
	Ui::MainWindowClass ui_;
};