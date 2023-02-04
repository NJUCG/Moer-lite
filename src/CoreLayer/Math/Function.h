#pragma once

//* 给定t以及范围[lower, upper]，将t的值限定在[lower, upper]内
template <typename T> T clamp(T t, T lower, T upper) {
  t = (t < lower) ? lower : (t > upper ? upper : t);
  return t;
}