#pragma once

#include <vector>
#include "Common/CommonTypes.h"
#include "SMS/RNGManipulator/RNGViewer.h"

class NameKuriRNG {
public:
  NameKuriRNG();
  NameKuriRNG(u32 seed);
  void set_seed(u32 seed);
  void set_index(u32 index);
  void set_search_range(const u32 search_range) {
    search_range_ = search_range;
  }
  void search();
  static QVariant header_data(s32 column);
  QVariant data(s32 row, s32 column) const;

  static struct Settings {
    struct {
      float min = 0.0f;
      float max = 1.0f;
    } distance = { 600.f, 1000.f },
      angle = { 0.f, 360.f },
      body_scale = { 0.6f, 1.0f },
      march_speed = { 1.0f, 1.5f };
  } settings_;

  struct Result {
    u32 seed;
    u32 index;
    bool has_7a5b_index;
    u32 index_7a;
    u32 index_5b;

    float distance;
    float angle;
    float body_scale;
    float march_speed;
  };
  std::vector<Result> results_;

private:
  u32 seed_ = 0;
  u32 index_ = 0;
  u32 search_range_ = 100000;
};
