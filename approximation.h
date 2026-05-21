#ifndef APPROXIMATION_H
#define APPROXIMATION_H

class domain;

class courant_field {
 public:
  static double evaluate (const double *coefficients, double x, double y,
                          const domain &domain);
};

#endif // APPROXIMATION_H
