#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef double (*BivariateFunction) (double, double);

class function_catalog {
 public:
  static BivariateFunction select (int index);
  static bool is_valid_index (int index);
};

#endif // FUNCTIONS_H
