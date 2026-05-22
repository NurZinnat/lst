#include "fem_core.h"

#include <cstring>

#include "approximation.h"
#include "errors.h"
#include "mesh_index.h"
#include "rhs_builder.h"
#include "solver_mr.h"
#include "stdio.h"
#include "synch.h"

execution_status
fem_core::init_fem_core (const domain &dom, int function_index,
                         double tolerance, int max_iterations, size_t p,
                         int *msr_index, double *msr_values, int msr_length,
                         double *rhs, double *solution, double *residual,
                         double *search, double *auxiliary)
{
  domain_ = dom;
  tolerance_ = tolerance;
  max_iterations_ = max_iterations;
  p_ = p;
  function_ = function_catalog::select (function_index);
  matrix_.attach (msr_index, msr_values, msr_length, domain_.node_count ());
  rhs_ = rhs;
  solution_ = solution;
  residual_ = residual;
  search_ = search;
  auxiliary_ = auxiliary;
  status_ = execution_status::success;
  return execution_status::success;
}

void
fem_core::worker_build_system (size_t thread_index)
{
  const int node_count = domain_.node_count ();
  int begin = 0;
  int end = 0;
  mesh_index::thread_range (node_count, static_cast<int> (p_),
                            static_cast<int> (thread_index), begin, end);

  std::memset (rhs_ + begin, 0, (end - begin) * sizeof (double));
  std::memset (solution_ + begin, 0, (end - begin) * sizeof (double));
  std::memset (residual_ + begin, 0, (end - begin) * sizeof (double));
  std::memset (search_ + begin, 0, (end - begin) * sizeof (double));
  std::memset (auxiliary_ + begin, 0, (end - begin) * sizeof (double));

  int error = 0;

  if (thread_index == 0)
    {
      const int pattern_length
          = matrix_.finalize_pattern (domain_.nx (), domain_.ny ());
      if (pattern_length != matrix_.length ())
        error = 1;
    }

  synchronization_reduce_int (p_, &error, 1);
  if (error != 0)
    {
      status_ = execution_status::runtime;
      return;
    }

  error = matrix_.fill_parallel (domain_, static_cast<int> (p_),
                                 static_cast<int> (thread_index));
  synchronization_reduce_int (p_, &error, 1);
  if (error != 0)
    {
      status_ = execution_status::runtime;
      return;
    }

  rhs_builder::fill_parallel (domain_, rhs_, function_, static_cast<int> (p_),
                              static_cast<int> (thread_index));
  synchronization_barrier (p_);

  extern double GetTime ();
  double build_start = 0.;
  synchronization_capture_time (p_, &build_start);

  const int iterations = min_residual_solver::solve_with_restarts (
      matrix_.values (), matrix_.index (), rhs_, solution_, residual_, search_,
      auxiliary_, tolerance_, max_iterations_, node_count,
      static_cast<int> (p_), static_cast<int> (thread_index));

  double build_end = 0.;
  synchronization_capture_time (p_, &build_end);
  build_time_ = build_end - build_start;

  int aggregated_iterations = iterations;
  synchronization_reduce_int (p_, &aggregated_iterations, 1);
  if (aggregated_iterations < 0)
    status_ = execution_status::small_divider;
  else
    iterations_ = aggregated_iterations;
}

void
fem_core::worker_compute_errors (size_t thread_index)
{
  extern double GetTime ();
  double error_start = 0.;
  synchronization_capture_time (p_, &error_start);

  r1_ = error_norms::max_centroid (domain_, solution_, function_,
                                   static_cast<int> (p_),
                                   static_cast<int> (thread_index));
  r2_ = error_norms::l1_centroid (domain_, solution_, function_,
                                  static_cast<int> (p_),
                                  static_cast<int> (thread_index));
  r3_ = error_norms::max_nodes (domain_, solution_, function_,
                                static_cast<int> (p_),
                                static_cast<int> (thread_index));
  r4_ = error_norms::l1_nodes (domain_, solution_, function_,
                               static_cast<int> (p_),
                               static_cast<int> (thread_index));

  double error_end = 0.;
  synchronization_capture_time (p_, &error_end);
  error_time_ = error_end - error_start;

  synchronization_barrier (p_);
}
