#include "functions.h"

#include <cmath>

#include "fem_task.h"

static double f0 (double, double) { return 1.; }
static double f1 (double x, double) { return x; }
static double f2 (double, double y) { return y; }
static double f3 (double x, double y) { return x + y; }
static double f4 (double x, double y) { return std::sqrt (x * x + y * y); }
static double f5 (double x, double y) { return x * x + y * y; }
static double f6 (double x, double y) { return std::exp (x * x - y * y); }
static double f7 (double x, double y)
{
  return 1. / (25. * (x * x + y * y) + 1.);
}

static BivariateFunction table[] = {f0, f1, f2, f3, f4, f5, f6, f7};

bool function_catalog::is_valid_index (int index)
{
  return index >= 0 && index <= MAX_FUNCTION_INDEX;
}

BivariateFunction function_catalog::select (int index)
{
  return table[index];
}
