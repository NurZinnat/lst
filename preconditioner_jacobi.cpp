#include "preconditioner.h"

#include "mesh_index.h"
#include "synch.h"

void jacobi_preconditioner::apply (int size, const double *diagonal, const double *residual,
                                  double *output, int thread_count, int thread_id)
{
  int begin = 0;
  int end = 0;
  mesh_index::thread_range (size, thread_count, thread_id, begin, end);

  for (int i = begin; i < end; ++i)
    output[i] = residual[i] / diagonal[i];

  synchronization_barrier (thread_count);
}
