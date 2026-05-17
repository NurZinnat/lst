#include "window.h"
Window::Window (QWidget *parent) : QWidget (parent)
{
  setWindowTitle ("Approximation");
  func_id = 0;
  f = get_func (func_id);
  f_name = get_func_name (func_id);
  display_mode = 0;
  p = 0;
  poly_k = 3;
  sp.memory_allocate ();
  spl.memory_allocate ();
}

void
Window::set_params (double _a, double _b, int _n, int _func_id)
{
  a = _a;
  b = _b;
  n = _n;
  func_id = _func_id;
  f = get_func (func_id);
  f_name = get_func_name (func_id);
  poly_k = func_id + 2;
  if (poly_k < 2)
    poly_k = 2;
  if (poly_k > MAX_K)
    poly_k = MAX_K;
  update_max_abs ();
  sp.set_new_params (n, a, b, poly_k, p * 0.1 * max_abs, f);
  spl.set_new_params (n, a, b, p * 0.1 * max_abs, f);
  spl.build ();
}

QPointF
Window::l2g (double x_loc, double y_loc, double y_min, double y_max)
{
  double x_gl = (x_loc - a) / (b - a) * width ();
  double y_gl = (y_max - y_loc) / (y_max - y_min) * height ();
  return QPointF (x_gl, y_gl);
}

void
Window::find_min_max (double &min_y, double &max_y)
{
  int W = width ();
  if (W <= 0)
    W = 1000;
  double hx = (b - a) / W;
  double mid_x = (a + b) / 2;
  double y0 = f (a);
  if (std::abs (a - mid_x) < 1e-12)
    y0 += p * 0.1 * max_abs;
  min_y = max_y = y0;
  for (int i = 0; i <= W; i++)
    {
      double x = a + i * hx;
      double y = f (x);
      if (std::abs (x - mid_x) < 1e-12)
        y += p * 0.1 * max_abs;
      if (y < min_y)
        min_y = y;
      if (y > max_y)
        max_y = y;
    }
}

void
Window::update_max_abs ()
{
  int W = width ();
  if (W <= 0)
    W = 1000;
  double hx = (b - a) / W;
  max_abs = 0;
  for (int i = 0; i <= W; i++)
    {
      double x = a + i * hx;
      double val = std::abs (f (x));
      if (val > max_abs)
        max_abs = val;
    }
}
