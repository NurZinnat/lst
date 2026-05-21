#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include "data.h"
#include "domain.h"
#include "execution_status.h"
#include <stddef.h>
#include <pthread.h>
#include <sys/time.h>

class thread_object;

double GetTime ();

class implementation
{
  domain domain_{};
  char *prg_name{};

  int nx{};
  int ny{};
  int function_index{};
  double tolerance{};
  int max_iterations{};
  size_t p = 0;

  data msr_index{};
  data msr_values{};
  data rhs{};
  data solution{};
  data residual{};
  data search{};
  data auxiliary{};
  data thread_partials{};

  int node_count{};
  int msr_length{};

  double r1{};
  double r2{};
  double r3{};
  double r4{};
  double t1{};
  double t2{};
  int iterations{};

  thread_object *thread_objs{};
  pthread_t *threads{};

  execution_status status{};

public:
  implementation () = default;
  ~implementation ();
  implementation (const implementation &x) = delete;
  implementation &operator= (const implementation &x) = delete;
  implementation (implementation &&x) = delete;
  implementation &operator= (implementation &&x) = delete;

  execution_status init_implementation (size_t argc, char *argv[]);
  execution_status memory_allocate_implementation ();

  domain &get_domain () { return domain_; }
  char *get_prg_name () const { return prg_name; }
  size_t get_p () const { return p; }
  int get_nx () const { return nx; }
  int get_ny () const { return ny; }
  int get_function_index () const { return function_index; }
  double get_tolerance () const { return tolerance; }
  int get_max_iterations () const { return max_iterations; }
  int *get_msr_index () { return reinterpret_cast<int *> (msr_index.get_arr ()); }
  double *get_msr_values () { return msr_values.get_arr (); }
  int get_msr_length () const { return msr_length; }
  double *get_rhs () { return rhs.get_arr (); }
  double *get_solution () { return solution.get_arr (); }
  double *get_residual () { return residual.get_arr (); }
  double *get_search () { return search.get_arr (); }
  double *get_auxiliary () { return auxiliary.get_arr (); }
  double *get_thread_partials () { return thread_partials.get_arr (); }

  void set_status (execution_status value) { status = value; }
  execution_status get_status () const { return status; }

  void set_r1 (double value) { r1 = value; }
  void set_r2 (double value) { r2 = value; }
  void set_r3 (double value) { r3 = value; }
  void set_r4 (double value) { r4 = value; }
  void set_t1 (double value) { t1 = value; }
  void set_t2 (double value) { t2 = value; }
  void set_iterations (int value) { iterations = value; }

  void GO (int argc, char *argv[]);
};

#endif // IMPLEMENTATION_H
