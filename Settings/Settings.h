#pragma once
#include <QObject>
#include <QSettings>

class Settings : public QObject {
  Q_OBJECT
public:
  Settings(const Settings&) = delete;
  Settings& operator=(const Settings&) = delete;
  Settings(Settings&&) = delete;
  Settings& operator=(Settings&&) = delete;

  ~Settings();

  static Settings& instance();
  static QSettings& GetQSettings();

  // Map Viewer
  void SetMapObjectViewerVisible(bool enabled);
  bool IsMapObjectViewerVisible() const;
  void SetMapObjectParametersVisible(bool enabled);
  bool IsMapObjectParametersVisible() const;
  void RefreshMapWidgetVisibility();

signals:
  void MapObjectViewerVisibilityChanged(bool visible);
  void MapObjectParametersVisibilityChanged(bool visible);

private:
  Settings();
};
