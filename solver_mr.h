#ifndef SOLVER_MR_H
#define SOLVER_MR_H

class sparse_algebra {
 public:
  static void multiply_msr (const double *matrix, const int *index, int size,
                            const double *input, double *output, int thread_count,
                            int thread_id);
  static void axpy_sub (int size, double *target, const double *direction, double scale,
                        int thread_count, int thread_id);
  static double dot (int size, const double *left, const double *right,
                     int thread_count, int thread_id);
};

class min_residual_solver {
 public:
  static int solve (double *matrix, const int *index, const double *rhs, double *solution,
                    double *residual, double *search, double *auxiliary, double tolerance,
                    int max_iterations, int size, int thread_count, int thread_id);

  static int solve_with_restarts (double *matrix, const int *index, const double *rhs,
                                  double *solution, double *residual, double *search,
                                  double *auxiliary, double tolerance, int max_iterations,
                                  int size, int thread_count, int thread_id);
};

#endif // SOLVER_MR_H
