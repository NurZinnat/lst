#include "errors.h"

#include <cmath>

#include "approximation.h"
#include "mesh_index.h"
#include "synch.h"

double error_norms::max_centroid (const domain &domain, const double *coefficients,
                                 BivariateFunction func, int thread_count, int thread_id)
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
  double local_max = 0.;

  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  for (int l = begin; l < end; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      if (i == nx || j == ny)
        continue;

      const double x1 = a + i * hx + hx / 3.;
      const double y1 = c + j * hy + 2. * hy / 3.;
      const double x2 = a + i * hx + 2. * hx / 3.;
      const double y2 = c + j * hy + hy / 3.;

      double delta = func (x1, y1) - courant_field::evaluate (coefficients, x1, y1, domain);
      local_max = std::max (local_max, std::fabs (delta));

      delta = func (x2, y2) - courant_field::evaluate (coefficients, x2, y2, domain);
      local_max = std::max (local_max, std::fabs (delta));
    }

  synchronization_partial_store (static_cast<size_t> (thread_count),
                                   static_cast<size_t> (thread_id), local_max);
  return synchronization_partial_max (static_cast<size_t> (thread_count));
}

double error_norms::l1_centroid (const domain &domain, const double *coefficients,
                                BivariateFunction func, int thread_count, int thread_id)
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
  double local_sum = 0.;

  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  for (int l = begin; l < end; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      if (i == nx || j == ny)
        continue;

      const double x1 = a + i * hx + hx / 3.;
      const double y1 = c + j * hy + 2. * hy / 3.;
      const double x2 = a + i * hx + 2. * hx / 3.;
      const double y2 = c + j * hy + hy / 3.;

      local_sum += std::fabs (func (x1, y1)
                              - courant_field::evaluate (coefficients, x1, y1, domain));
      local_sum += std::fabs (func (x2, y2)
                              - courant_field::evaluate (coefficients, x2, y2, domain));
    }

  synchronization_partial_store (static_cast<size_t> (thread_count),
                                   static_cast<size_t> (thread_id),
                                   local_sum * hx * hy * 0.5);
  return synchronization_partial_sum (static_cast<size_t> (thread_count));
}

double error_norms::max_nodes (const domain &domain, const double *coefficients,
                              BivariateFunction func, int thread_count, int thread_id)
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
  double local_max = 0.;

  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  for (int l = begin; l < end; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      if (i == nx || j == ny)
        continue;

      const double delta = func (a + i * hx, c + j * hy) - coefficients[l];
      local_max = std::max (local_max, std::fabs (delta));
    }

  synchronization_partial_store (static_cast<size_t> (thread_count),
                                   static_cast<size_t> (thread_id), local_max);
  return synchronization_partial_max (static_cast<size_t> (thread_count));
}

double error_norms::l1_nodes (const domain &domain, const double *coefficients,
                             BivariateFunction func, int thread_count, int thread_id)
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
  double local_sum = 0.;

  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  for (int l = begin; l < end; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      if (i == nx || j == ny)
        continue;

      local_sum += std::fabs (func (a + i * hx, c + j * hy) - coefficients[l]);
    }

  synchronization_partial_store (static_cast<size_t> (thread_count),
                                   static_cast<size_t> (thread_id), local_sum * hx * hy);
  return synchronization_partial_sum (static_cast<size_t> (thread_count));
}
