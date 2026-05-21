#ifndef FEM_CORE_H
#define FEM_CORE_H

#include "domain.h"
#include "execution_status.h"
#include "functions.h"
#include "msr_matrix.h"
#include <stddef.h>

class fem_core
{
  domain domain_{};
  msr_matrix matrix_{};
  BivariateFunction function_{};

  double *rhs_{};
  double *solution_{};
  double *residual_{};
  double *search_{};
  double *auxiliary_{};

  double tolerance_{};
  int max_iterations_{};
  size_t p_ = 0;

  double r1_{};
  double r2_{};
  double r3_{};
  double r4_{};
  double build_time_{};
  double error_time_{};
  int iterations_{};

  execution_status status_{};

public:
  fem_core () = default;
  ~fem_core () = default;
  fem_core (const fem_core &x) = delete;
  fem_core &operator= (const fem_core &x) = delete;
  fem_core (fem_core &&x) = delete;
  fem_core &operator= (fem_core &&x) = delete;

  execution_status init_fem_core (const domain &dom, int function_index,
                                  double tolerance, int max_iterations, size_t p,
                                  int *msr_index, double *msr_values, int msr_length,
                                  double *rhs, double *solution, double *residual,
                                  double *search, double *auxiliary);

  const domain &get_domain () const { return domain_; }
  size_t get_p () const { return p_; }

  double get_r1 () const { return r1_; }
  double get_r2 () const { return r2_; }
  double get_r3 () const { return r3_; }
  double get_r4 () const { return r4_; }
  double get_build_time () const { return build_time_; }
  double get_error_time () const { return error_time_; }
  int get_iterations () const { return iterations_; }
  execution_status get_status () const { return status_; }
  void set_status (execution_status value) { status_ = value; }

  void worker_build_system (size_t thread_index);
  void worker_compute_errors (size_t thread_index);
};

#endif // FEM_CORE_H
