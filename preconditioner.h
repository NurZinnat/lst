#ifndef PRECONDITIONER_H
#define PRECONDITIONER_H

class jacobi_preconditioner {
 public:
  static void apply (int size, const double *diagonal, const double *residual,
                     double *output, int thread_count, int thread_id);
};

#endif // PRECONDITIONER_H
