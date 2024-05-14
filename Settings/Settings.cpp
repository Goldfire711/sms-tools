#include "Settings.h"

Settings::Settings() = default;

Settings::~Settings() = default;

Settings& Settings::instance() {
  static Settings settings;
  return settings;
}

QSettings& Settings::GetQSettings() {
  static QSettings settings("settings.ini", QSettings::IniFormat);
  return settings;
}
