#pragma once

#include "../../Common/CommonTypes.h"

namespace rng_functions
{
  // base^exp mod 2^64
  inline u64 pow_mod64(u64 base, u32 exp)
  {
    u64 result = 1;
    while (exp > 0)
    {
      if (exp & 1)
        result *= base;
      base *= base;
      exp >>= 1;
    }
    return result;
  }

  // https://qiita.com/drken/items/3b4fdf0a78e7a138cd9a
  // a^(-1) mod m を求める
  constexpr s64 modular_inverse(s64 a, s64 m)
  {
    s64 b = m, u = 1, v = 0;
    while (b)
    {
      long long t = a / b;
      a -= t * b;
      //std::swap(a, b);
      s64 c = a;
      a = b;
      b = c;
      u -= t * v;
      //std::swap(u, v);
      s64 w = u;
      u = v;
      v = w;
    }
    u %= m;
    if (u < 0)
      u += m;
    return u;
  }

  // [0 .. 2^32-1] -> [0, 1)
  inline float seed_to_float_value(const u32 seed)
  {
    return static_cast<float>(seed >> 0x10 & 0x7fff) / 32768;
  }

  inline void seed_next(u32* seed)
  {
    *seed = *seed * 0x41c64e6d + 0x3039;
  }

  inline u32 index_to_seed(const u32 index)
  {
    // 0x3039 * ((0x41c64e6d^index - 1) / 4) * ((0x41c64e6d - 1) / 4)^(-1) (mod 2^32) (モジュラ逆数を使用)
    constexpr u32 mod_inv = modular_inverse((0x41c64e6d - 1) / 4, 0x100000000);
    return static_cast<u32>(0x3039 * ((pow_mod64(0x41c64e6d, index) - 1) / 4) * mod_inv);
  }

  inline u32 seed_to_index(const u32 seed)
  {
    u32 index_guess = 0;
    u32 seed_guess = 0;
    for (u32 mask = 1; mask != 0; mask <<= 1)
    {
      if ((seed & mask) != (seed_guess & mask))
      {
        index_guess += mask;
        seed_guess = index_to_seed(index_guess);
      }
    }
    return index_guess;
  }

  // 7a+5b=indexを満たす0以上の整数a,bの組の中で、bが最小のものを
  // a,bに代入 存在しない場合falseを返す
  inline bool index_to_7a5b(u32 index, u32* a, u32* b)
  {
    // index<24の場合、a,bの組が存在しない場合がある
    if (index < 24)
    {
      for (u32 x = 0; x <= (24 / 7); x++)
      {
        for (u32 y = 0; y <= (24 / 5); y++)
        {
          if (index == 7 * x + 5 * y)
          {
            *a = x;
            *b = y;
            return true;
          }
        }
      }
    }
    else
      // index>=24の場合、a,bの組は必ず1つ以上存在する
    {
      constexpr u32 mod_inv = modular_inverse(5, 7);
      *b = index * mod_inv % 7;   // 5b = index (mod 7) を満たすb
      *a = (index - 5 * *b) / 7;  // 7a = index - 5b を満たすa (index - 5b は↑より7で割った余りは0になる)
      return true;
    }
    return false;
  }

  inline bool next_7a5b(u32* a, u32* b)
  {
    if (*a >= 5)
    {
      *a = *a - 5;
      *b = *b + 7;
      return true;
    }
    return false;
  }

  inline bool prev_7a5b(u32* a, u32* b)
  {
    if (*b >= 7)
    {
      *a = *a + 5;
      *b = *b - 7;
      return true;
    }
    return false;
  }

} // namespace rng_functions
