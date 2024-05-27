#include <deque>

#include "NameKuriManipulator.h"
#include "SMS/RNGManipulator/RNGFunctions.h"

NameKuriRNG::Settings NameKuriRNG::settings_;

NameKuriRNG::NameKuriRNG() = default;

NameKuriRNG::NameKuriRNG(const u32 seed) {
  set_seed(seed);
}

void NameKuriRNG::set_seed(const u32 seed) {
  seed_ = seed;
  index_ = rng::seed_to_index(seed);
}

void NameKuriRNG::set_index(const u32 index) {
  index_ = index;
  seed_ = rng::index_to_seed(index);
}

void NameKuriRNG::search() {
  results_.clear();

  u32 seed = seed_;
  std::deque<float> rng_values = { INFINITY };
  for (u32 i = 0; i < 5; i++) {
    rng_values.push_back(rng::seed_to_float(seed));
    rng::seed_next(&seed);
  }

  const bool angle_wrap = settings_.angle.min > settings_.angle.max;

  for (u32 i = 0; i < search_range_; i++) {
    rng_values.pop_front();
    rng_values.push_back(rng::seed_to_float(seed));
    rng::seed_next(&seed);

    const float distance = 600.0f + rng_values[0] * 400.0f;
    if (distance < settings_.distance.min || settings_.distance.max < distance)
      continue;

    const float angle = rng_values[1] * 360.f;
    if (angle_wrap) {
      if (settings_.angle.max < angle && angle < settings_.angle.min)
        continue;
    } else {
      if (angle < settings_.angle.min || settings_.angle.max < angle)
        continue;
    }

    if (2.f < rng_values[2])
      continue;

    // const float turn_speed = 0.2 + rng_values[3] * 0; (skip)

    const float body_scale = 0.6f + rng_values[4] * 0.4f;
    if (body_scale < settings_.body_scale.min || settings_.body_scale.max < body_scale)
      continue;

    const float march_speed = 1.0f + rng_values[5] * 0.5f;
    if (march_speed < settings_.march_speed.min || settings_.march_speed.max < march_speed)
      continue;

    u32 _7a, _5b;
    const bool has_7a5b = rng::index_to_7a5b(i, &_7a, &_5b);
    results_.push_back({ rng::seed_prev(seed), i, has_7a5b, _7a, _5b, distance, angle, body_scale, march_speed });
  }
  results_.shrink_to_fit();
}

QVariant NameKuriRNG::header_data(const s32 column) {
  switch (column) {
  case 0:
    return "Index+";
  case 1:
    return "Drops";
  case 2:
    return "Distance";
  case 3:
    return "Angle";
  case 4:
    return "Body scale";
  case 5:
    return "March speed";
  case 6:
    return "RNG Seed";
  default:
    return {};
  }
}

QVariant NameKuriRNG::data(const s32 row, const s32 column) const {
  Result result = results_[row];
  switch (column) {
  case 0:
    return result.index;
  case 1:
    if (result.has_7a5b_index)
      return QString("(%1 %2)").arg(result.index_7a).arg(result.index_5b);
    return {};
  case 2:
    return result.distance;
  case 3:
    return result.angle;
  case 4:
    return result.body_scale;
  case 5:
    return result.march_speed;
  case 6:
    return result.seed;
  default:
    return {};
  }
}
