#pragma once

#include <vector>
#include "Common/CommonTypes.h"

struct RNGResult {
  u32 index;
  bool has_7a5b_index;
  u32 index_7a;
  u32 index_5b;
  float value;
  u32 seed;
};

// TODO •¡”ŒÂÁ”ï‚·‚é—”‚É‘Î‰‚·‚é
// void append(float min, float max, QString type, QString name)‚Å’Ç‰Á
// void set_search_range(u32)

class RNG {
public:
  RNG();
  RNG(u32 seed);
  void set_seed(u32 seed);
  void set_index(u32 index);
  void search_rng(float search_min, float search_max, float rng_min, float rng_max, u32 search_range = 100000);

  u32 seed_ = 0;
  u32 index_ = 0;
  std::vector<RNGResult> results_;
  double probability_inv_ = 1;
};