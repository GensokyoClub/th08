#pragma once
#include "inttypes.hpp"

#include <d3dx8math.h>
#include <math.h>

#include "Float3.hpp"

#define ZUN_MIN(x, y) ((x) > (y) ? (y) : (x))
#define ZUN_PI ((f32)(3.14159265358979323846))
#define ZUN_2PI ((f32)(ZUN_PI * 2.0f))

#define sincos(in, out_sine, out_cosine)                                                                               \
    {                                                                                                                  \
        __asm { \
        __asm fld in \
        __asm fsincos \
        __asm fstp out_cosine \
        __asm fstp out_sine }                                            \
    }

namespace th08
{

static void IncrementIfBelow(u32 *value, u32 threshold)
{
    if (*value < threshold)
    {
        (*value)++;
    }
}

} // namespace th08
