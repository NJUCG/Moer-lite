#pragma once
#include "RGB.h"

//* 可以在编译的时候根据预编译宏定义选择渲染时不同的颜色空间
//* 编程时请使用Spectrum作为接口
//* 目前只实现了RGB三通道光谱
//* 如果要实现采样光谱，请实现与RGB相同的接口

using Spectrum = SpectrumRGB;