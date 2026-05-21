#include "domain.h"

domain::domain (double a, double b, double c, double d, int nx, int ny)
    : a_ (a), b_ (b), c_ (c), d_ (d), nx_ (nx), ny_ (ny), hx_ ((b - a) / nx),
      hy_ ((d - c) / ny)
{
}
