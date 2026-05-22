#include "rhs_builder.h"
#include "mesh_index.h"

void
rhs_builder::fill_parallel (const domain &domain, double *rhs,
                            BivariateFunction func, int thread_count,
                            int thread_id)
{
  const int nx = domain.nx ();
  const int ny = domain.ny ();
  const int node_count = domain.node_count ();
  const double hx = domain.hx ();
  const double hy = domain.hy ();
  const double a = domain.a ();
  const double c = domain.c ();

  int begin = 0, end = 0;
  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  const double diag_coef = hx * hy / 12.0;
  const double off_coef = hx * hy / 24.0;

  for (int l = begin; l < end; ++l)
    {
      int i = 0, j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      const double xc = a + i * hx;
      const double yc = c + j * hy;
      const double v00 = func (xc, yc);

      const double v_r = (i < nx) ? func (xc + hx, yc) : 0.0;
      const double v_l = (i > 0) ? func (xc - hx, yc) : 0.0;
      const double v_u = (j < ny) ? func (xc, yc + hy) : 0.0;
      const double v_d = (j > 0) ? func (xc, yc - hy) : 0.0;

      const double v_ru = (i < nx && j < ny) ? func (xc + hx, yc + hy) : 0.0;
      const double v_ld = (i > 0 && j > 0) ? func (xc - hx, yc - hy) : 0.0;

      int hits = 0;
      if (i < nx && j > 0)
        hits += 1;
      if (i > 0 && j > 0)
        hits += 2;
      if (i > 0 && j < ny)
        hits += 1;
      if (i < nx && j < ny)
        hits += 2;

      double sum = v00 * hits * diag_coef;

      if (i < nx && j > 0)
        sum += (v_r + v_d) * off_coef;
      if (i > 0 && j > 0)
        sum += (v_d + v_ld * 2 + v_l) * off_coef;
      if (i > 0 && j < ny)
        sum += (v_l + v_u) * off_coef;
      if (i < nx && j < ny)
        sum += (v_u + v_ru * 2 + v_r) * off_coef;

      rhs[l] += sum;
    }
}