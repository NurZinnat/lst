#include "approximation.h"

#include "domain.h"
#include "mesh_index.h"

double courant_field::evaluate (const double *coefficients, double x, double y,
                               const domain &domain)
{
  const int nx = domain.nx ();
  const double a = domain.a ();
  const double c = domain.c ();
  const double hx = domain.hx ();
  const double hy = domain.hy ();

  const int i = static_cast<int> ((x - a) / hx);
  const int j = static_cast<int> ((y - c) / hy);

  if (y - hy * (x - (a + i * hx)) / hx - (c + j * hy) <= 0.)
    {
      const int s1 = mesh_index::ij_to_l (nx, i, j);
      const int s2 = mesh_index::ij_to_l (nx, i + 1, j);
      const int s3 = mesh_index::ij_to_l (nx, i + 1, j + 1);
      return -coefficients[s1] * (x - a - (i + 1) * hx) / hx
             + coefficients[s2]
                   * (1. + (x - a - (i + 1) * hx) / hx - (y - c - j * hy) / hy)
             + coefficients[s3] * (y - c - j * hy) / hy;
    }

  const int s1 = mesh_index::ij_to_l (nx, i, j);
  const int s2 = mesh_index::ij_to_l (nx, i + 1, j + 1);
  const int s3 = mesh_index::ij_to_l (nx, i, j + 1);
  return -coefficients[s1] * (y - c - (j + 1) * hy) / hy
         + coefficients[s2] * (x - a - i * hx) / hx
         + coefficients[s3]
               * (1. - (x - a - i * hx) / hx + (y - c - (j + 1) * hy) / hy);
}
