// Standalone tests for sliding Newton polynomial (method 1).
// Build: g++ -O2 -std=c++17 -o test_sliding test_sliding.cpp sliding_polynomial.cpp functions.cpp

#include "sliding_polynomial.h"
#include "functions.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>

static double
mesh_x (double a, double b, size_t n, ptrdiff_t i)
{
  return a + (b - a) * static_cast<double> (i) / static_cast<double> (n);
}

// Max |P(x_i)-f(x_i)| on all mesh nodes (incremental window, O(n) shifts).
static double
max_node_interp_error (sliding_polynomial &sp, double a, double b, size_t n,
                       size_t k, double (*f) (double))
{
  sp.calc_rightmost_coeffs ();
  const double h = (b - a) / static_cast<double> (n);
  double max_e = 0;

  // Right end: window anchored at the right.
  for (ptrdiff_t gi = static_cast<ptrdiff_t> (n);
       gi >= static_cast<ptrdiff_t> (n + 1 - static_cast<ptrdiff_t> (k)); --gi)
    {
      const double x = mesh_x (a, b, n, gi);
      const double e = std::abs (sp.eval (x) - f (x));
      if (e > max_e)
        max_e = e;
    }

  for (ptrdiff_t gi = static_cast<ptrdiff_t> (n - static_cast<ptrdiff_t> (k));
       gi >= 0; --gi)
    {
      sp.calc_next_coeffs ();
      const double x = mesh_x (a, b, n, gi);
      (void)h;
      const double e = std::abs (sp.eval (x) - f (x));
      if (e > max_e)
        max_e = e;
    }
  return max_e;
}

// As in methods_draw.cpp after fix.
static void
scan_draw_like (sliding_polynomial &sp, double a, double b, size_t n, int W,
                std::vector<double> &xs, std::vector<double> &ys)
{
  sp.calc_rightmost_coeffs ();
  const double hx = (b - a) / W;
  const double h = (b - a) / static_cast<double> (n);
  xs.assign (static_cast<size_t> (W + 1), 0.0);
  ys.assign (static_cast<size_t> (W + 1), 0.0);

  const double x_prev = b;
  ys[static_cast<size_t> (W)] = sp.eval (x_prev);
  xs[static_cast<size_t> (W)] = x_prev;

  for (int i = W - 1; i >= 0; --i)
    {
      const double x_curr = a + i * hx;
      while (i > 0 && x_curr < sp.get_current_left () - 0.5 * h)
        sp.calc_next_coeffs ();
      xs[static_cast<size_t> (i)] = x_curr;
      ys[static_cast<size_t> (i)] = sp.eval (x_curr);
    }
}

// Correct sweep b->a: shift until x_curr is inside current window.
static void
scan_correct_shifts (sliding_polynomial &sp, double a, double b, size_t n,
                     int W, std::vector<double> &xs, std::vector<double> &ys)
{
  sp.calc_rightmost_coeffs ();
  const double hx = (b - a) / W;
  const double h = (b - a) / static_cast<double> (n);

  xs.assign (static_cast<size_t> (W + 1), 0.0);
  ys.assign (static_cast<size_t> (W + 1), 0.0);

  for (int i = W; i >= 0; --i)
    {
      const double x = a + i * hx;
      xs[static_cast<size_t> (i)] = x;
      while (x < sp.get_current_left () - 0.5 * h)
        sp.calc_next_coeffs ();
      ys[static_cast<size_t> (i)] = sp.eval (x);
    }
}

static double
max_abs_err (const std::vector<double> &xs, const std::vector<double> &ys,
             double (*f) (double))
{
  double m = 0;
  for (size_t i = 0; i < xs.size (); ++i)
    {
      const double e = std::abs (ys[i] - f (xs[i]));
      if (e > m)
        m = e;
    }
  return m;
}

static int
count_jumps (const std::vector<double> &ys, double thr)
{
  int c = 0;
  for (size_t i = 1; i < ys.size (); ++i)
    if (std::abs (ys[i] - ys[i - 1]) > thr)
      c++;
  return c;
}

int
main (int argc, char **argv)
{
  const double a = -1.0;
  const double b = 1.0;
  const int func_id = (argc > 1) ? std::atoi (argv[1]) : 3;
  const int W = (argc > 2) ? std::atoi (argv[2]) : 1000;

  double (*f) (double) = get_func (func_id);
  size_t poly_k = static_cast<size_t> (func_id + 2);
  if (poly_k < 2)
    poly_k = 2;
  if (poly_k > MAX_K)
    poly_k = MAX_K;

  std::vector<size_t> ns = { 50, 100, 200, 500, 1000, 2000, 5000, 10000,
                               20000, 50000, 100000, 200000, 524388 };

  printf ("func_id=%d poly_k=%zu W=%d [%.3f, %.3f]\n", func_id, poly_k, W, a,
          b);
  printf (
      "%10s %10s %14s %14s %14s %8s %8s\n",
      "n", "h", "err_nodes", "err_draw", "err_correct", "jumps_d",
      "jumps_c");
  printf (
      "%10s %10s %14s %14s %14s %8s %8s\n",
      "----------", "----------", "--------------", "--------------",
      "--------------", "--------", "--------");

  sliding_polynomial sp;
  sp.memory_allocate ();

  std::vector<double> xs, y_draw, y_correct;

  for (size_t n : ns)
    {
      if (n < 2)
        continue;
      sp.set_new_params (n, a, b, poly_k, 0.0, f);
      const double h = (b - a) / static_cast<double> (n);

      const double max_node
          = max_node_interp_error (sp, a, b, n, poly_k, f);

      scan_draw_like (sp, a, b, n, W, xs, y_draw);
      scan_correct_shifts (sp, a, b, n, W, xs, y_correct);

      printf (
          "%10zu %10.3e %14.3e %14.3e %14.3e %8d %8d\n",
          n, h, max_node, max_abs_err (xs, y_draw, f),
          max_abs_err (xs, y_correct, f), count_jumps (y_draw, 0.5),
          count_jumps (y_correct, 0.5));
    }

  // When does draw mode diverge from correct shifts?
  printf ("\n--- draw vs correct: first n with err_draw > 1 ---\n");
  for (size_t n = 100; n <= 200000; n *= 2)
    {
      sp.set_new_params (n, a, b, poly_k, 0.0, f);
      scan_draw_like (sp, a, b, n, W, xs, y_draw);
      scan_correct_shifts (sp, a, b, n, W, xs, y_correct);
      const double ed = max_abs_err (xs, y_draw, f);
      const double ec = max_abs_err (xs, y_correct, f);
      if (ed > 1.0 || ec > 1.0)
        printf ("n=%zu err_draw=%.3e err_correct=%.3e ratio=%.2f hx/h=%.1f\n",
                n, ed, ec, ed / (ec + 1e-300),
                ((b - a) / W) / ((b - a) / static_cast<double> (n)));
    }

  return 0;
}
