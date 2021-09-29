#pragma once
#include "FluffRNGFunctions.h"

#include <QCheckBox>

class SMSData
{
public:
  std::vector<fluff_rng_functions::FluffResult> m_fluffsResult;
  fluff_rng_functions::Rectangular m_bounds = { {-5000, 0, -5000}, {5000, 9139, 5000} };
  fluff_rng_functions::Rectangular m_range;
  fluff_rng_functions::Vector3d m_target;
  float m_targetSwingDirection = 0;
  Qt::CheckState search_for_origin = Qt::Unchecked;

  bool map_viewer_is_active = false;
};
