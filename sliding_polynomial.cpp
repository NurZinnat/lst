#include "sliding_polynomial.h"

#include <cmath>
#include <memory>

void
sliding_polynomial::memory_allocate ()
{
  coeff = std::make_unique<double[]> (MAX_K);
  points = std::make_unique<double[]> (MAX_K);
}

void
sliding_polynomial::set_new_params (size_t _n, double _a, double _b, size_t _k,
                                    double _pert, double (*_func) (double))
{
  n = _n;

  a = _a;
  b = _b;

  k = _k;

  if (k > n + 1)
    k = n + 1;

  pert = _pert;

  func = _func;
}

void
sliding_polynomial::calc_rightmost_coeffs ()
{
  first_point_index = static_cast<ptrdiff_t> (n + 1 - k);

  calc_current_coeffs ();
}

void
sliding_polynomial::calc_next_coeffs ()
{
  // сдвиг окна ВЛЕВО
  if (first_point_index > 0)
    first_point_index--;

  calc_current_coeffs ();
}

void
sliding_polynomial::calc_current_coeffs ()
{
  double *points_arr = points.get ();
  double *coeff_arr = coeff.get ();

  const double h = (b - a) / static_cast<double> (n);

  const size_t mid_idx = n / 2;

  // ------------------------------------------------------------
  // Узлы и значения функции
  // ------------------------------------------------------------

  for (size_t i = 0; i < k; ++i)
    {
      ptrdiff_t global_idx = first_point_index + static_cast<ptrdiff_t> (i);

      double x_global = get_x (global_idx);

      // локальные координаты
      points_arr[i] = static_cast<double> (i) * h;

      coeff_arr[i] = func (x_global);

      if (global_idx >= 0 && static_cast<size_t> (global_idx) == mid_idx)
        {
          coeff_arr[i] += pert;
        }
    }

  // ------------------------------------------------------------
  // Разделенные разности
  // ------------------------------------------------------------

  for (size_t order = 1; order < k; ++order)
    {
      for (int j = static_cast<int> (k) - 1; j >= static_cast<int> (order);
           --j)
        {
          double denominator = points_arr[j] - points_arr[j - order];

          coeff_arr[j] = (coeff_arr[j] - coeff_arr[j - 1]) / denominator;
        }
    }
}

double
sliding_polynomial::eval (double x) const
{
  const double *c = coeff.get ();
  const double *p = points.get ();

  // БАЗА ТОГО ЖЕ ОКНА
  double base_x = get_x (first_point_index);

  // локальная координата
  double t = x - base_x;

  double result = c[k - 1];

  for (int j = static_cast<int> (k) - 2; j >= 0; --j)
    {
      result = result * (t - p[j]) + c[j];
    }

  return result;
}

double
sliding_polynomial::get_current_left () const
{
  return get_x (first_point_index);
}