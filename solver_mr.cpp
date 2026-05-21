#include "solver_mr.h"

#include <cmath>

#include "fem_task.h"
#include "mesh_index.h"
#include "synch.h"
#include "preconditioner.h"

void sparse_algebra::multiply_msr (const double *matrix, const int *index, int size,
                                  const double *input, double *output, int thread_count,
                                  int thread_id)
{
  int begin = 0;
  int end = 0;
  mesh_index::thread_range (size, thread_count, thread_id, begin, end);

  for (int row = begin; row < end; ++row)
    {
      double sum = matrix[row] * input[row];
      const int row_start = index[row];
      const int row_length = index[row + 1] - row_start;

      for (int offset = 0; offset < row_length; ++offset)
        sum += matrix[row_start + offset] * input[index[row_start + offset]];

      output[row] = sum;
    }

  synchronization_barrier (thread_count);
}

void sparse_algebra::axpy_sub (int size, double *target, const double *direction, double scale,
                              int thread_count, int thread_id)
{
  int begin = 0;
  int end = 0;
  mesh_index::thread_range (size, thread_count, thread_id, begin, end);

  for (int i = begin; i < end; ++i)
    target[i] -= scale * direction[i];

  synchronization_barrier (thread_count);
}

double sparse_algebra::dot (int size, const double *left, const double *right,
                           int thread_count, int thread_id)
{
  int begin = 0;
  int end = 0;
  mesh_index::thread_range (size, thread_count, thread_id, begin, end);

  double local = 0.;
  for (int i = begin; i < end; ++i)
    local += left[i] * right[i];

  synchronization_partial_store (static_cast<size_t> (thread_count),
                                   static_cast<size_t> (thread_id), local);
  return synchronization_partial_sum (thread_count);
}

int min_residual_solver::solve (double *matrix, const int *index, const double *rhs,
                              double *solution, double *residual, double *search,
                              double *auxiliary, double tolerance, int max_iterations,
                              int size, int thread_count, int thread_id)
{
  const double rhs_norm2 = sparse_algebra::dot (size, rhs, rhs, thread_count, thread_id);
  const double stop1 = rhs_norm2 * tolerance * tolerance;
  const double stop2 = rhs_norm2 * rhs_norm2 * tolerance * tolerance;

  sparse_algebra::multiply_msr (matrix, index, size, solution, residual, thread_count,
                               thread_id);
  sparse_algebra::axpy_sub (size, residual, rhs, 1., thread_count, thread_id);

  int iteration = 0;
  for (; iteration < max_iterations; ++iteration)
    {
      jacobi_preconditioner::apply (size, matrix, residual, search, thread_count, thread_id);
      sparse_algebra::multiply_msr (matrix, index, size, search, auxiliary, thread_count,
                                   thread_id);

      const double numerator
          = sparse_algebra::dot (size, auxiliary, residual, thread_count, thread_id);
      const double denominator
          = sparse_algebra::dot (size, auxiliary, auxiliary, thread_count, thread_id);

      if (std::fabs (numerator) < stop1 || std::fabs (denominator) < stop2)
        break;

      const double step = numerator / denominator;
      sparse_algebra::axpy_sub (size, solution, search, step, thread_count, thread_id);
      sparse_algebra::axpy_sub (size, residual, auxiliary, step, thread_count, thread_id);
    }

  synchronization_barrier (thread_count);
  return iteration >= max_iterations ? -1 : iteration;
}

int min_residual_solver::solve_with_restarts (double *matrix, const int *index,
                                            const double *rhs, double *solution,
                                            double *residual, double *search,
                                            double *auxiliary, double tolerance,
                                            int max_iterations, int size,
                                            int thread_count, int thread_id)
{
  int total_iterations = 0;

  for (int attempt = 0; attempt < MAX_SOLVER_RESTARTS; ++attempt)
    {
      const int iterations
          = solve (matrix, index, rhs, solution, residual, search, auxiliary, tolerance,
                   max_iterations, size, thread_count, thread_id);
      if (iterations >= 0)
        return total_iterations + iterations;

      total_iterations += max_iterations;
    }

  return -1;
}
