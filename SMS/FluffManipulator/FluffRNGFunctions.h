#pragma once
#include "../../Common/CommonTypes.h"

#include <vector>
#include <QCheckBox>

namespace fluff_rng_functions
{
  struct Vector3d
  {
    float x;
    float y;
    float z;
  };

  struct Rectangular
  {
    Vector3d min;
    Vector3d max;
  };

  struct FluffResult
  {
    struct U32_ab
    {
      u32 a;
      u32 b;
    };
    u32 index_from_seed_0;
    s32 index;
    U32_ab index_7a5b;
    bool has_7a5b_index;
    float x_origin;
    float z_origin;
    float phase;
    float x;
    float y;
    float z;
    float x_from_target;
    float y_from_target;
    float z_from_target;
    u32 rng_seed;
  };

  std::vector<FluffResult> searchFluffs(u32 seed, Rectangular area, float xz_degree, Vector3d target, u32 search_range, Qt::CheckState search_for_origin);
} // namespace fluff_rng_functions