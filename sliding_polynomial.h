#ifndef SLIDING_POLYNOMIAL
#define SLIDING_POLYNOMIAL

#include <cstddef>
#include <memory>

#define MAX_K 10

class sliding_polynomial
{
  size_t n{};
  size_t k{};

  double a{}, b{};

  std::unique_ptr<double[]> coeff{};
  std::unique_ptr<double[]> points{};

  // индекс левого узла текущего окна
  ptrdiff_t first_point_index{};

  double (*func) (double) = nullptr;

  double pert{};

public:
  sliding_polynomial () = default;
  ~sliding_polynomial () = default;

  sliding_polynomial (const sliding_polynomial &) = delete;
  sliding_polynomial (sliding_polynomial &&) = delete;

  sliding_polynomial &operator= (const sliding_polynomial &) = delete;

  sliding_polynomial &operator= (sliding_polynomial &&) = delete;

  void memory_allocate ();

  void set_new_params (size_t _n, double _a, double _b, size_t _k,
                       double _pert, double (*_func) (double));

  void calc_rightmost_coeffs ();

  void calc_next_coeffs ();

  double eval (double x) const;

  double get_current_left () const;

  size_t
  get_n () const
  {
    return n;
  }

private:
  void calc_current_coeffs ();

  double
  get_x (ptrdiff_t i) const
  {
    return a + (b - a) * static_cast<double> (i) / static_cast<double> (n);
  }
};

#endif // SLIDING_POLYNOMIAL