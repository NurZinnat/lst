#ifndef DOMAIN_H
#define DOMAIN_H

class domain
{
  double a_{};
  double b_{};
  double c_{};
  double d_{};
  int nx_{};
  int ny_{};
  double hx_{};
  double hy_{};

public:
  domain () = default;
  domain (double a, double b, double c, double d, int nx, int ny);

  double a () const { return a_; }
  double b () const { return b_; }
  double c () const { return c_; }
  double d () const { return d_; }
  int nx () const { return nx_; }
  int ny () const { return ny_; }
  double hx () const { return hx_; }
  double hy () const { return hy_; }
  int node_count () const { return (nx_ + 1) * (ny_ + 1); }
};

#endif // DOMAIN_H
