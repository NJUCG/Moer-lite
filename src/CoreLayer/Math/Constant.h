#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <limits>

#ifdef FLT_MAX
#undef FLT_MAX
#endif // FLT_MAX

constexpr float PI = M_PI;
constexpr float INV_PI = 1.f / M_PI;
constexpr float FLT_MAX = std::numeric_limits<float>::max();
constexpr float EPSILON = 1e-4f; // 暂定，后续再做细致分析
