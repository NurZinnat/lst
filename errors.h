#ifndef ERRORS_H
#define ERRORS_H

#include "domain.h"
#include "functions.h"

class error_norms {
 public:
  static double max_centroid (const domain &domain, const double *coefficients,
                              BivariateFunction func, int thread_count, int thread_id);

  static double l1_centroid (const domain &domain, const double *coefficients,
                             BivariateFunction func, int thread_count, int thread_id);

  static double max_nodes (const domain &domain, const double *coefficients,
                           BivariateFunction func, int thread_count, int thread_id);

  static double l1_nodes (const domain &domain, const double *coefficients,
                          BivariateFunction func, int thread_count, int thread_id);
};

#endif // ERRORS_H
