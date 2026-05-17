#include "window.h"
#include <QPainter>
#include <cmath>

void
Window::draw_method1 (QPainter &painter, double min_y, double max_y)
{
  int W = width ();
  if (n < 2)
    return;

  sp.calc_rightmost_coeffs ();
  const double hx = (b - a) / W;
  const double h = (b - a) / n;
  double x_prev = b, y_prev = sp.eval (x_prev);

  for (int i = W - 1; i >= 0; i--)
    {
      const double x_curr = a + i * hx;
      while (i > 0 && x_curr < sp.get_current_left () - 0.5 * h)
        sp.calc_next_coeffs ();
      const double y_curr = sp.eval (x_curr);
      painter.drawLine (l2g (x_prev, y_prev, min_y, max_y),
                        l2g (x_curr, y_curr, min_y, max_y));
      x_prev = x_curr;
      y_prev = y_curr;
    }
}

void
Window::draw_method2 (QPainter &painter, double min_y, double max_y)
{
  int W = width ();
  if (n < 2)
    return;

  double hx = (b - a) / W;
  double x_prev = b, y_prev = spl.eval (x_prev);

  for (int i = W - 1; i >= 0; i--)
    {
      double x_curr = a + i * hx;
      double y_curr = spl.eval (x_curr);
      painter.drawLine (l2g (x_prev, y_prev, min_y, max_y),
                        l2g (x_curr, y_curr, min_y, max_y));
      x_prev = x_curr;
      y_prev = y_curr;
    }
}

void
Window::draw_method1_error (QPainter &painter, double min_y, double max_y)
{
  int W = width ();
  if (n < 2)
    return;

  sp.calc_rightmost_coeffs ();
  const double hx = (b - a) / W;
  const double h = (b - a) / n;
  const double mid_x = (a + b) / 2;

  double x_prev = b;
  double y_approx = sp.eval (x_prev);
  double y_func = f (x_prev);
  if (std::abs (x_prev - mid_x) < 1e-12)
    y_func += p * 0.1 * max_abs;
  double y_prev = std::abs (y_approx - y_func);

  painter.setPen (QPen (Qt::blue, 2));
  for (int i = W - 1; i >= 0; i--)
    {
      const double x_curr = a + i * hx;
      while (i > 0 && x_curr < sp.get_current_left () - 0.5 * h)
        sp.calc_next_coeffs ();
      y_approx = sp.eval (x_curr);
      y_func = f (x_curr);
      if (std::abs (x_curr - mid_x) < 1e-12)
        y_func += p * 0.1 * max_abs;
      const double err = std::abs (y_approx - y_func);
      painter.drawLine (l2g (x_prev, y_prev, min_y, max_y),
                        l2g (x_curr, err, min_y, max_y));
      x_prev = x_curr;
      y_prev = err;
    }
}

void
Window::draw_method2_error (QPainter &painter, double min_y, double max_y)
{
  int W = width ();
  if (n < 2)
    return;

  double hx = (b - a) / W;
  double mid_x = (a + b) / 2;

  double x_prev = b;
  double y_approx = spl.eval (x_prev);
  double y_func = f (x_prev);
  if (std::abs (x_prev - mid_x) < 1e-12)
    y_func += p * 0.1 * max_abs;
  double y_prev = std::abs (y_approx - y_func);

  QPen pen (QColor (255, 105, 180), 2); // розовый
  painter.setPen (pen);

  for (int i = W - 1; i >= 0; i--)
    {
      double x_curr = a + i * hx;
      y_approx = spl.eval (x_curr);
      y_func = f (x_curr);
      if (std::abs (x_curr - mid_x) < 1e-12)
        y_func += p * 0.1 * max_abs;
      double err = std::abs (y_approx - y_func);
      painter.drawLine (l2g (x_prev, y_prev, min_y, max_y),
                        l2g (x_curr, err, min_y, max_y));
      x_prev = x_curr;
      y_prev = err;
    }
}