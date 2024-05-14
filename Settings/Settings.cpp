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

void Settings::SetMapObjectViewerVisible(const bool enabled) {
  if (IsMapObjectViewerVisible() != enabled) {
    GetQSettings().setValue("map/show_object_viewer", enabled);
    emit MapObjectViewerVisibilityChanged(enabled);
  }
}

bool Settings::IsMapObjectViewerVisible() const {
  if (const auto value = GetQSettings().value("map/show_object_viewer"); value.isValid())
    return value.toBool();
  return true;
}

void Settings::SetMapObjectParametersVisible(const bool enabled) {
  if (IsMapObjectParametersVisible() != enabled) {
    GetQSettings().setValue("map/show_object_parameters", enabled);
    emit MapObjectParametersVisibilityChanged(enabled);
  }
}

bool Settings::IsMapObjectParametersVisible() const {
  if (const auto value = GetQSettings().value("map/show_object_parameters"); value.isValid())
    return value.toBool();
  return true;
}

void Settings::RefreshMapWidgetVisibility() {
  emit MapObjectViewerVisibilityChanged(IsMapObjectViewerVisible());
  emit MapObjectParametersVisibilityChanged(IsMapObjectParametersVisible());
}
