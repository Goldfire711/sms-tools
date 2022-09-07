#pragma once
#include <QString>

#include "Common/CommonTypes.h"

#include <vector>

struct Result {
  u32 index;
  bool has_7a5b_index;
  u32 index_7a;
  u32 index_5b;
  float value;
  u32 seed;
};

class ChuuHanaRNG {
public:
  ChuuHanaRNG();
  ChuuHanaRNG(u32 seed);
  void set_seed(u32 seed);
  void set_index(u32 index);
  void search_rng_m30_30(float min, float max, u32 search_range = 10000);
  void search_rng_0_7(float min, float max, u32 search_range = 10000);
  void search_rng_0_8(float min, float max, u32 search_range = 10000);
  void search_rng_0_8_2(const std::vector<s32> &vertices, u32 search_range = 10000);

  u32 seed_ = 0;
  u32 index_ = 0;
  std::vector<Result> results_;
  enum {
    RNG_m30_30,
    RNG_0_7,
    RNG_0_8
  } rng_type_;
  double probability_inv_;
};
