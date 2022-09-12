#include "ChuuHanaRNG.h"
#include "RNGFunctions.h"
#include <QDebug>

ChuuHanaRNG::ChuuHanaRNG() {
  seed_ = 0;
  index_ = 0;
}

ChuuHanaRNG::ChuuHanaRNG(u32 seed) {
  set_seed(seed);
}

void ChuuHanaRNG::set_seed(u32 seed) {
  seed_ = seed;
  index_ = rng::seed_to_index(seed);
}

void ChuuHanaRNG::set_index(u32 index) {
  index_ = index;
  seed_ = rng::index_to_seed(index);
}

void ChuuHanaRNG::search_rng_m30_30(float min, float max, u32 search_range) {
  rng_type_ = RNG_m30_30;
  probability_inv_ = (30.0 - -30.0) / (max - min);
  u32 seed = seed_;
  results_.clear();
  results_.shrink_to_fit();
  for (u32 i = 0; i < search_range; i++) {
    rng::seed_next(&seed);
    float value = -30.0f + 60.0f * rng::seed_to_float(seed);
    if (min <= value && value <= max) {
      Result result;
      result.seed = rng::seed_prev(seed);
      result.value = value;
      result.index = i;
      result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
      results_.emplace_back(result);
    }
  }
}

void ChuuHanaRNG::search_rng_0_7(float min, float max, u32 search_range) {
  rng_type_ = RNG_0_7;
  probability_inv_ = static_cast<s64>((7.0f - 0.0f) / (max - min));
  u32 seed = seed_;
  results_.clear();
  results_.shrink_to_fit();
  for (u32 i = 0; i < search_range; i++) {
    rng::seed_next(&seed);
    float value = floorf(7.0f * rng::seed_to_float(seed));
    if (min <= value && value <= max) {
      Result result;
      result.seed = rng::seed_prev(seed);
      result.value = value;
      result.index = i;
      result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
      results_.emplace_back(result);
    }
  }
}

void ChuuHanaRNG::search_rng_0_8(float min, float max, u32 search_range) {
  rng_type_ = RNG_0_8;
  probability_inv_ = static_cast<s64>((8.0f - 0.0f) / (max - min));
  u32 seed = seed_;
  results_.clear();
  results_.shrink_to_fit();
  for (u32 i = 0; i < search_range; i++) {
    rng::seed_next(&seed);
    float value = floorf(8.0f * rng::seed_to_float(seed));
    if (min <= value && value <= max) {
      Result result;
      result.seed = rng::seed_prev(seed);
      result.value = value;
      result.index = i;
      result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
      results_.emplace_back(result);
    }
  }
}

void ChuuHanaRNG::search_rng_0_8_2(const std::vector<s32> &vertices, u32 search_range) {
  rng_type_ = RNG_0_8;
  probability_inv_ = (8.0 - 0.0) / static_cast<double>(vertices.size());
  u32 seed = seed_;
  results_.clear();
  results_.shrink_to_fit();
  for (u32 i = 0; i < search_range; i++) {
    rng::seed_next(&seed);
    const float value = floorf(8.0f * rng::seed_to_float(seed));
    if (std::ranges::binary_search(vertices.begin(), vertices.end(), static_cast<s32>(value))) {
      Result result;
      result.seed = rng::seed_prev(seed);
      result.value = value;
      result.index = i;
      result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
      results_.emplace_back(result);
    }
  }
}

void ChuuHanaRNG::search_rng_int_array(const s32 rng_min, const s32 rng_max, const bool is_collided, const std::vector<s32> &search_array, const u32 search_range) {
  rng_type_ = RNG_INT;
  const float rng_range_f = static_cast<float>(rng_max + 1 - rng_min);
  const float rng_min_f = static_cast<float>(rng_min);
  probability_inv_ = static_cast<double>(rng_range_f) / static_cast<double>(search_array.size());
  u32 seed = seed_;
  results_.clear();
  results_.shrink_to_fit();
  if (is_collided) {
    // isCollidMove
    probability_inv_ *= 4.0;
    u32 seed2 = rng::seed_next(seed);
    for (u32 i = 0; i < search_range; i++) {
      rng::seed_next(&seed);
      if (static_cast<s32>((100.0f * rng::seed_to_float(seed))) % 4 == 0) {
        seed2 = rng::seed_next(seed);
        const s32 value = static_cast<s32>(rng_min_f + rng_range_f * rng::seed_to_float(seed2));
        if (std::ranges::binary_search(search_array.begin(), search_array.end(), value)) {
          Result result;
          result.seed = seed;
          result.value = static_cast<float>(value);
          result.index = i;
          result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
          results_.emplace_back(result);
        }
      }
    }
  } else {
    // willFall
    for (u32 i = 0; i < search_range; i++) {
      rng::seed_next(&seed);
      const s32 value = static_cast<s32>(rng_min_f + rng_range_f * rng::seed_to_float(seed));
      if (std::ranges::binary_search(search_array.begin(), search_array.end(), value)) {
        Result result;
        result.seed = rng::seed_prev(seed);
        result.value = static_cast<float>(value);
        result.index = i;
        result.has_7a5b_index = rng::index_to_7a5b(result.index, &result.index_7a, &result.index_5b);
        results_.emplace_back(result);
      }
    }
  }
}
