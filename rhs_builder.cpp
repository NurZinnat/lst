#include "rhs_builder.h"

#include "mesh_index.h"

void rhs_builder::fill_parallel (const domain &domain, double *rhs, BivariateFunction func,
                               int thread_count, int thread_id)
{
  const int nx = domain.nx ();
  const int ny = domain.ny ();
  const int node_count = domain.node_count ();
  const double hx = domain.hx ();
  const double hy = domain.hy ();
  const double a = domain.a ();
  const double c = domain.c ();

  int begin = 0;
  int end = 0;
  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  for (int l = begin; l < end; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      const int weight = ((i < nx && j > 0 ? 1 : 0) + (i > 0 && j > 0 ? 2 : 0)
                          + (i > 0 && j < ny ? 1 : 0) + (i < nx && j < ny ? 2 : 0));

      double sum = func (a + i * hx, c + j * hy) * weight * hx * hy / 12.;
      sum += (((i < nx && j > 0)
                   ? (func (a + (i + 1) * hx, c + j * hy)
                      + func (a + i * hx, c + (j - 1) * hy))
                   : 0.)
              + ((i > 0 && j > 0)
                     ? (func (a + i * hx, c + (j - 1) * hy)
                        + func (a + (i - 1) * hx, c + (j - 1) * hy)
                        + func (a + (i - 1) * hx, c + (j - 1) * hy)
                        + func (a + (i - 1) * hx, c + j * hy))
                     : 0.)
              + ((i > 0 && j < ny)
                     ? (func (a + (i - 1) * hx, c + j * hy)
                        + func (a + i * hx, c + (j + 1) * hy))
                     : 0.)
              + ((i < nx && j < ny)
                     ? (func (a + i * hx, c + (j + 1) * hy)
                        + func (a + (i + 1) * hx, c + (j + 1) * hy)
                        + func (a + (i + 1) * hx, c + (j + 1) * hy)
                        + func (a + (i + 1) * hx, c + j * hy))
                     : 0.))
             * hx * hy / 24.;

      rhs[l] += sum;
    }
}
