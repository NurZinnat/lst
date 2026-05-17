#include "spline.h"

#include <cmath>
#include <memory>

void
spline::memory_allocate ()
{
  d = std::make_unique<double[]> (MAX_N + 1);
}

void
spline::set_new_params (size_t _n, double _a, double _b, double _pert,
                        double (*_func) (double))
{
  n = _n;
  a = _a;
  b = _b;

  pert = _pert;
  func = _func;
}

double
spline::eval (double x) const
{
  if (n < 2)
    return func (x);

  const double hv = h ();

  const size_t mid = n / 2;

  auto fval = [&] (size_t i) -> double
    {
      double v = func (get_x (i));

      if (i == mid)
        v += pert;

      return v;
    };

  // Ограничение области

  if (x <= a)
    x = a;

  if (x >= b)
    x = b;

  // Номер интервала

  size_t i = (size_t)((x - a) / hv);

  if (i >= n)
    i = n - 1;

  // Локальная координата

  const double t = (x - get_x (i)) / hv;

  const double t2 = t * t;

  const double t3 = t2 * t;

  // ============================================================
  // Кубический базис Эрмита
  // ============================================================

  const double H00 = 2.0 * t3 - 3.0 * t2 + 1.0;

  const double H10 = -2.0 * t3 + 3.0 * t2;

  const double H01 = t3 - 2.0 * t2 + t;

  const double H11 = t3 - t2;

  return fval (i) * H00 + fval (i + 1) * H10 + d[i] * hv * H01
         + d[i + 1] * hv * H11;
}

void
spline::build ()
{
  if (n < 2)
    return;

  const double hv = h ();

  const size_t mid = n / 2;

  auto fval = [&] (size_t i) -> double
    {
      double v = func (get_x (i));

      if (i == mid)
        v += pert;

      return v;
    };

  // ============================================================
  // n == 2
  //
  // Точность до x^2
  // ============================================================

  if (n == 2)
    {
      d[0] = (-3.0 * fval (0) + 4.0 * fval (1) - fval (2)) / (2.0 * hv);

      d[1] = (fval (2) - fval (0)) / (2.0 * hv);

      d[2] = (3.0 * fval (2) - 4.0 * fval (1) + fval (0)) / (2.0 * hv);

      return;
    }

  // ============================================================
  // Граничные производные Бесселя
  //
  // Точность до x^3
  // ============================================================

  d[0] = (-11.0 * fval (0) + 18.0 * fval (1) - 9.0 * fval (2) + 2.0 * fval (3))
         / (6.0 * hv);

  d[n] = (11.0 * fval (n) - 18.0 * fval (n - 1) + 9.0 * fval (n - 2)
          - 2.0 * fval (n - 3))
         / (6.0 * hv);

  // ============================================================
  // n == 3
  // ============================================================

  if (n == 3)
    {
      d[1] = (fval (2) - fval (0)) / (2.0 * hv);

      d[2] = (fval (3) - fval (1)) / (2.0 * hv);

      return;
    }

  // ============================================================
  // Прогонка
  // ============================================================

  auto c = std::make_unique<double[]> (n + 1);

  auto e = std::make_unique<double[]> (n + 1);

  // Первое уравнение

  double rhs = 3.0 * (fval (2) - fval (0)) / hv - d[0];

  c[1] = 1.0 / 4.0;

  e[1] = rhs / 4.0;

  // Внутренние уравнения

  for (size_t i = 2; i <= n - 2; ++i)
    {
      rhs = 3.0 * (fval (i + 1) - fval (i - 1)) / hv;

      const double denom = 4.0 - c[i - 1];

      c[i] = 1.0 / denom;

      e[i] = (rhs - e[i - 1]) / denom;
    }

  // Последнее уравнение

  rhs = 3.0 * (fval (n) - fval (n - 2)) / hv - d[n];

  d[n - 1] = (rhs - e[n - 2]) / (4.0 - c[n - 2]);

  // Обратная прогонка

  for (int i = (int)n - 2; i >= 1; --i)
    {
      d[i] = e[i] - c[i] * d[i + 1];
    }
}