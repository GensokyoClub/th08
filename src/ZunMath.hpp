#pragma once
#include <math.h>
#include "inttypes.hpp"

#define ZUN_MIN(x, y) ((x) > (y) ? (y) : (x))
#define ZUN_PI ((f32)(3.14159265358979323846))
#define ZUN_2PI ((f32)(ZUN_PI * 2.0f))

// Redundant typecast required for matching assembly
#define ZUN_TAN(X) ((f32) tan(X))