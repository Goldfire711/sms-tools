#include "FluffRNGFunctions.h"
#include "RNGFunctions.h"


#include <iostream>
#include <vector>

namespace fluff_rng_functions
{
  std::vector<FluffResult> searchFluffs(u32 seed, const Rectangular area, const float xz_degree,
    const Vector3d target, const u32 search_range,
    Qt::CheckState search_for_origin)
  {
    std::vector<FluffResult> fluffs_result;
    const u32 first_index = rng_functions::seed_to_index(seed);
    const float xz_sin = sinf(3.14f * xz_degree / 180.0f);
    const float xz_cos = cosf(3.14f * xz_degree / 180.0f);

    std::vector<float> rng_values;
    rng_values.reserve(search_range);
    for (u32 i = 0; i < 5; ++i)
    {
      rng_values.emplace_back(rng_functions::seed_to_float_value(seed));
      rng_functions::seed_next(&seed);
    }
    // forループ. float_value値を1個ずつ確保しながら、z,y,x,(風抵抗),位相を最新の5つの
    // rng_values配列から算出して、area内にあればxyz値等を結果の配列に追加. search_range分回す
    // search_for_origin(原点を検索)がCheckedの場合、初期phaseを考慮しないで揺れの原点を検索
    // Uncheckedの場合、初期phaseを考慮したxyzを検索（これが実際のワープ後の座標になる）
    if (search_for_origin == Qt::Checked)
    {

      for (u32 index = 0; index < search_range; ++index)
      {
        FluffResult fluff;
        fluff.z = 5000.0f * (2.0f * rng_values.at(index) - 1.0f);
        fluff.y = 9139.11523f * rng_values.at(index + 1);
        fluff.x = 5000.0f * (2.0f * rng_values.at(index + 2) - 1.0f);
        //fluff.phase = 360.0f * rng_values.at(index + 4);

        //fluff.x = fluff.x_origin + 300 * sinf(3.14f * fluff.phase / 180.0f) * (xz_cos + xz_sin);
        //fluff.z = fluff.z_origin+ 300 * sinf(3.14f * fluff.phase / 180.0f) * (xz_sin - xz_cos);

        if ((area.min.x <= fluff.x && fluff.x <= area.max.x) &&
          (area.min.y <= fluff.y && fluff.y <= area.max.y) &&
          (area.min.z <= fluff.z && fluff.z <= area.max.z))
        {
          fluff.index = index - 1;
          fluff.index_from_seed_0 = first_index + fluff.index;
          fluff.rng_seed = rng_functions::index_to_seed(fluff.index_from_seed_0);
          fluff.has_7a5b_index =
            rng_functions::index_to_7a5b(fluff.index, &fluff.index_7a5b.a, &fluff.index_7a5b.b);

          fluff.x_from_target = fluff.x - target.x;
          fluff.y_from_target = fluff.y - target.y;
          fluff.z_from_target = fluff.z - target.z;

          fluffs_result.emplace_back(fluff);
        }

        rng_values.emplace_back(rng_functions::seed_to_float_value(seed));
        rng_functions::seed_next(&seed);
      }
    }
    else
    {
      for (u32 index = 0; index < search_range; ++index)
      {
        FluffResult fluff;
        fluff.z_origin = 5000.0f * (2.0f * rng_values.at(index) - 1.0f);
        fluff.y = 9139.11523f * rng_values.at(index + 1);
        fluff.x_origin = 5000.0f * (2.0f * rng_values.at(index + 2) - 1.0f);
        fluff.phase = 360.0f * rng_values.at(index + 4);

        fluff.x = fluff.x_origin + 300 * sinf(3.14f * fluff.phase / 180.0f) * (xz_cos + xz_sin);
        fluff.z = fluff.z_origin + 300 * sinf(3.14f * fluff.phase / 180.0f) * (xz_sin - xz_cos);

        if ((area.min.x <= fluff.x && fluff.x <= area.max.x) &&
          (area.min.y <= fluff.y && fluff.y <= area.max.y) &&
          (area.min.z <= fluff.z && fluff.z <= area.max.z))
        {
          fluff.index = index - 1;
          fluff.index_from_seed_0 = first_index + fluff.index;
          fluff.rng_seed = rng_functions::index_to_seed(fluff.index_from_seed_0);
          fluff.has_7a5b_index =
            rng_functions::index_to_7a5b(fluff.index, &fluff.index_7a5b.a, &fluff.index_7a5b.b);

          fluff.x_from_target = fluff.x - target.x;
          fluff.y_from_target = fluff.y - target.y;
          fluff.z_from_target = fluff.z - target.z;

          fluffs_result.emplace_back(fluff);
        }

        rng_values.emplace_back(rng_functions::seed_to_float_value(seed));
        rng_functions::seed_next(&seed);
      }
    }

    return fluffs_result;
  }
}// namespace fluff_rng_functions