#include "RNG.h"
#include "RNGFunctions.h"

RNG::RNG() {
  seed_ = 0;
  index_ = 0;
}

RNG::RNG(u32 seed) {
  set_seed(seed);
}

void RNG::set_seed(u32 seed) {
  seed_ = seed;
  index_ = rng::seed_to_index(seed);
}

void RNG::set_index(u32 index) {
  index_ = index;
  seed_ = rng::index_to_seed(index);
}

void RNG::search_rng(float search_min, float search_max, float rng_min, float rng_max, u32 search_range) {
  probability_inv_ = (rng_max - rng_min) / (search_max - search_min);
  u32 seed = seed_;
  results_.clear();
  results_.shrink_to_fit();
  for (u32 i = 0; i < search_range; i++) {
    rng::seed_next(&seed);
    float value = rng_min + (rng_max - rng_min) * rng::seed_to_float(seed);
    if (search_min <= value && value <= search_max) {
      RNGResult result;
      result.seed = rng::seed_prev(seed);
      result.value = value;
      result.index = i;
      result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
      results_.emplace_back(result);
    }
  }
}
