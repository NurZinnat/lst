#ifndef MSR_MATRIX_H
#define MSR_MATRIX_H

class domain;

class msr_matrix {
 public:
  msr_matrix () = default;

  void attach (int *index, double *storage, int length, int node_count);

  int length () const { return length_; }
  int *index () { return index_; }
  const int *index () const { return index_; }
  double *values () { return storage_; }
  const double *values () const { return storage_; }

  static int nonzero_estimate (int nx, int ny);
  static int build_index_pattern (int nx, int ny, int *index);
  static int count_off_diagonal (int nx, int ny, int i, int j);

  int fill_diagonal_entry (int nx, int ny, double hx, double hy, int i, int j,
                           double *value) const;
  int fill_off_diagonal_entries (int nx, int ny, double hx, double hy, int i, int j,
                                 int *index, double *values) const;

  int fill_parallel (const domain &domain, int thread_count, int thread_id);
  int finalize_pattern (int nx, int ny);

 private:
  int *index_ = nullptr;
  double *storage_ = nullptr;
  int length_ = 0;
  int node_count_ = 0;
};

#endif // MSR_MATRIX_H
