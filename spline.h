#ifndef SPLINE_H
#define SPLINE_H

#include <memory>

#define MAX_N 268435456

class spline
{
  size_t n{};
  double a{}, b{};
  double (*func) (double);
  double pert{};
  std::unique_ptr<double[]> d;

public:
  spline () = default;
  ~spline () = default;
  spline (const spline &) = delete;
  spline (spline &&) = delete;
  spline &operator= (const spline &) = delete;
  spline &operator= (spline &&) = delete;

  void memory_allocate ();
  void set_new_params (size_t _n, double _a, double _b, double _pert,
                       double (*_func) (double));
  void build ();
  double eval (double x) const;
  size_t
  get_n () const
  {
    return n;
  }

private:
  double
  h () const
  {
    return (b - a) / n;
  }
  double
  get_x (size_t i) const
  {
    return a + i * h ();
  }
};

#endif