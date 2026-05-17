#include "window.h"
#include <cmath>

double Window::calc_method1_error ()
{
  int W = width ();
  if (n < 2) return 0;

  sp.calc_rightmost_coeffs ();
  const double hx = (b - a) / W;
  const double h = (b - a) / n;
  const double mid_x = (a + b) / 2;

  double max_err = 0;
  for (int i = W - 1; i >= 0; i--)
    {
      const double x = a + i * hx;
      while (i > 0 && x < sp.get_current_left () - 0.5 * h)
        sp.calc_next_coeffs ();
      const double y_approx = sp.eval (x);
      double y_func = f (x);
      if (std::abs (x - mid_x) < 1e-12)
        y_func += p * 0.1 * max_abs;
      const double err = std::abs (y_approx - y_func);
      if (err > max_err)
        max_err = err;
    }
  return max_err;
}

double Window::calc_method2_error ()
{
  int W = width ();
  if (n < 2) return 0;

  double hx = (b - a) / W;
  double mid_x = (a + b) / 2;

  double max_err = 0;
  for (int i = W - 1; i >= 0; i--)
    {
      double x = a + i * hx;
      double y_approx = spl.eval (x);
      double y_func = f (x);
      if (std::abs (x - mid_x) < 1e-12) y_func += p * 0.1 * max_abs;
      double err = std::abs (y_approx - y_func);
      if (err > max_err) max_err = err;
    }
  return max_err;
}