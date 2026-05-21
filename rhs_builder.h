#ifndef RHS_BUILDER_H
#define RHS_BUILDER_H

#include "domain.h"
#include "functions.h"

class rhs_builder {
 public:
  static void fill_parallel (const domain &domain, double *rhs, BivariateFunction func,
                             int thread_count, int thread_id);
};

#endif // RHS_BUILDER_H
