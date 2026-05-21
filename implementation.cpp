#include "implementation.h"

#include "fem_task.h"
#include "functions.h"
#include "msr_matrix.h"
#include "synch.h"
#include "thread_object.h"

#include <cstdio>
#include <cstdlib>

double
GetTime ()
{
  struct timeval t;
  gettimeofday (&t, 0);
  return t.tv_sec + t.tv_usec / 1e6;
}

implementation::~implementation ()
{
  if (thread_objs)
    delete[] thread_objs;
  if (threads)
    delete[] threads;
}

execution_status
implementation::init_implementation (size_t argc, char *argv[])
{
  if (argc != 11)
    {
      printf ("usage: %s a b c d nx ny k eps maxit p\n", argv[0]);
      return execution_status::inval_arg;
    }

  prg_name = argv[0];

  double a = 0.;
  double b = 0.;
  double c = 0.;
  double d = 0.;
  int pp = 0;

  if (sscanf (argv[1], "%lf", &a) != 1 || sscanf (argv[2], "%lf", &b) != 1
      || sscanf (argv[3], "%lf", &c) != 1 || sscanf (argv[4], "%lf", &d) != 1
      || sscanf (argv[5], "%d", &nx) != 1 || sscanf (argv[6], "%d", &ny) != 1
      || sscanf (argv[7], "%d", &function_index) != 1
      || sscanf (argv[8], "%lf", &tolerance) != 1
      || sscanf (argv[9], "%d", &max_iterations) != 1
      || sscanf (argv[10], "%d", &pp) != 1)
    {
      printf ("usage: %s a b c d nx ny k eps maxit p\n", argv[0]);
      return execution_status::inval_arg;
    }

  if (b <= a || d <= c || nx < 1 || ny < 1 || max_iterations < 1 || pp < 1
      || !function_catalog::is_valid_index (function_index))
    {
      printf ("usage: %s a b c d nx ny k eps maxit p\n", argv[0]);
      return execution_status::inval_arg;
    }

  domain_ = domain (a, b, c, d, nx, ny);
  p = static_cast<size_t> (pp);
  node_count = domain_.node_count ();
  msr_length = node_count + 1 + msr_matrix::nonzero_estimate (nx, ny);

  return execution_status::success;
}

execution_status
implementation::memory_allocate_implementation ()
{
  execution_status st = msr_index.memory_allocate (static_cast<size_t> (msr_length));
  if (st != execution_status::success)
    return st;

  st = msr_values.memory_allocate (static_cast<size_t> (msr_length));
  if (st != execution_status::success)
    return st;

  st = rhs.memory_allocate (static_cast<size_t> (node_count));
  if (st != execution_status::success)
    return st;

  st = solution.memory_allocate (static_cast<size_t> (node_count));
  if (st != execution_status::success)
    return st;

  st = residual.memory_allocate (static_cast<size_t> (node_count));
  if (st != execution_status::success)
    return st;

  st = search.memory_allocate (static_cast<size_t> (node_count));
  if (st != execution_status::success)
    return st;

  st = auxiliary.memory_allocate (static_cast<size_t> (node_count));
  if (st != execution_status::success)
    return st;

  st = thread_partials.memory_allocate (p);
  if (st != execution_status::success)
    return st;

  synchronization_partial_bind (thread_partials.get_arr (), p);

  return execution_status::success;
}

void
implementation::GO (int argc, char *argv[])
{
  if (init_implementation (argc, argv) != execution_status::success)
    return;
  if (memory_allocate_implementation () != execution_status::success)
    {
      printf ("No memory\n");
      return;
    }

  if (p > 1)
    {
      threads = new pthread_t[p - 1];
      if (!threads)
        {
          printf ("can not allocate memory for threads len = %ld\n", p - 1);
          return;
        }
    }

  thread_objs = new thread_object[p];
  if (!thread_objs)
    {
      printf ("can not allocate memory for thread_objs len = %ld\n", p);
      return;
    }

  for (size_t i = 0; i < p; i++)
    thread_objs[i].init_thread_object (i, this);

  for (size_t i = 0; i < p - 1; i++)
    pthread_create (&threads[i], NULL, worker_thread, &thread_objs[i + 1]);

  thread_objs[0].GO ();

  for (size_t i = 0; i < p - 1; i++)
    pthread_join (threads[i], NULL);

  switch (status)
    {
    case execution_status::success:
      break;
    case execution_status::small_divider:
      printf ("Algorithm is not applicable\n");
      break;
    default:
      printf ("runtime error\n");
      break;
    }

  if (status == execution_status::success)
    {
      r1 = thread_objs[0].get_r1 ();
      r2 = thread_objs[0].get_r2 ();
      r3 = thread_objs[0].get_r3 ();
      r4 = thread_objs[0].get_r4 ();
      t1 = thread_objs[0].get_build_time ();
      t2 = thread_objs[0].get_error_time ();
      iterations = thread_objs[0].get_iterations ();

      printf (
          "%s : Task = %d R1 = %e R2 = %e R3 = %e R4 = %e T1 = %.2f T2 = %.2f "
          "It = %d E = %e K = %d Nx = %d Ny = %d P = %d\n",
          prg_name, TASK_ID, r1, r2, r3, r4, t1, t2, iterations, tolerance,
          function_index, nx, ny, static_cast<int> (p));
    }
}
