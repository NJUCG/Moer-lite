#pragma once

//* 给定t以及范围[lower, upper]，将t的值限定在[lower, upper]内
template <typename T> T clamp(T t, T lower, T upper) {
  t = (t < lower) ? lower : (t > upper ? upper : t);
  return t;
}

//* 求解一元二次方程组
inline bool Quadratic(float A, float B, float C, float *t0, float *t1) {
    float discrim = B * B - 4 *  A  * C;
    if (discrim < 0.) return false;
    float rootDiscrim = std::sqrt(discrim);
    float q;
    if (B < 0)
        q = -.5f * (B - rootDiscrim);
    else
        q = -.5f * (B + rootDiscrim);
    *t0 = q / A;
    *t1 = C / q;
    if ((float)*t0 > (float)*t1) std::swap(*t0, *t1);
    return true;
}