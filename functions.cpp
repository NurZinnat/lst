#include "functions.h"
#include <cmath>

double f_0 (double) { return 1; }
double f_1 (double x) { return x; }
double f_2 (double x) { return x * x; }
double f_3 (double x) { return x * x * x; }
double f_4 (double x) { return x * x * x * x; }
double f_5 (double x) { return exp (x); }
double f_6 (double x) { return 1.0 / (25.0 * x * x + 1.0); }

const char *
get_func_name (int k)
{
  switch (k)
    {
    case 0: return "f(x) = 1";
    case 1: return "f(x) = x";
    case 2: return "f(x) = x^2";
    case 3: return "f(x) = x^3";
    case 4: return "f(x) = x^4";
    case 5: return "f(x) = e^x";
    case 6: return "f(x) = 1/(25x^2 + 1)";
    default: return "";
    }
}

double (*get_func (int k)) (double)
{
  switch (k)
    {
    case 0: return f_0;
    case 1: return f_1;
    case 2: return f_2;
    case 3: return f_3;
    case 4: return f_4;
    case 5: return f_5;
    case 6: return f_6;
    default: return nullptr;
    }
}