#include "synch.h"

#include "implementation.h"

#include <algorithm>
#include <pthread.h>

void
synchronization_barrier (size_t p)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t input_count = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t output_count = PTHREAD_COND_INITIALIZER;
  static size_t input_number = 0;
  static size_t output_number = 0;

  if (p <= 1)
    return;

  pthread_mutex_lock (&mutex);
  input_number++;
  if (input_number >= p)
    {
      output_number = 0;
      pthread_cond_broadcast (&input_count);
    }
  else
    while (input_number < p)
      pthread_cond_wait (&input_count, &mutex);

  output_number++;
  if (output_number >= p)
    {
      input_number = 0;
      pthread_cond_broadcast (&output_count);
    }
  else
    while (output_number < p)
      pthread_cond_wait (&output_count, &mutex);

  pthread_mutex_unlock (&mutex);
}

void
synchronization_reduce_int (size_t p, int *value, size_t length)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t input_count = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t output_count = PTHREAD_COND_INITIALIZER;
  static size_t input_number = 0;
  static size_t output_number = 0;
  static int *active_value = nullptr;

  if (p <= 1)
    return;

  pthread_mutex_lock (&mutex);
  if (active_value == nullptr)
    active_value = value;
  else if (value != nullptr)
    {
      for (size_t i = 0; i < length; i++)
        active_value[i] += value[i];
    }

  input_number++;
  if (input_number >= p)
    {
      output_number = 0;
      pthread_cond_broadcast (&input_count);
    }
  else
    while (input_number < p)
      pthread_cond_wait (&input_count, &mutex);

  if (value != nullptr && active_value != value)
    {
      for (size_t i = 0; i < length; i++)
        value[i] = active_value[i];
    }

  output_number++;
  if (output_number >= p)
    {
      input_number = 0;
      active_value = nullptr;
      pthread_cond_broadcast (&output_count);
    }
  else
    while (output_number < p)
      pthread_cond_wait (&output_count, &mutex);

  pthread_mutex_unlock (&mutex);
}

static double *partial_values = nullptr;
static size_t partial_count = 0;

void
synchronization_partial_bind (double *buffer, size_t p)
{
  partial_values = buffer;
  partial_count = p;
}

void
synchronization_partial_store (size_t /*p*/, size_t thread_index, double value)
{
  partial_values[thread_index] = value;
}

double
synchronization_partial_sum (size_t p)
{
  double sum = 0.;
  synchronization_barrier (p);
  for (size_t i = 0; i < partial_count; i++)
    sum += partial_values[i];
  synchronization_barrier (p);
  return sum;
}

double
synchronization_partial_max (size_t p)
{
  double maximum = 0.;
  synchronization_barrier (p);
  for (size_t i = 0; i < partial_count; i++)
    if (partial_values[i] > maximum)
      maximum = partial_values[i];
  synchronization_barrier (p);
  return maximum;
}

bool
synchronization_error_check (size_t p, execution_status thread_status,
                             implementation *imp)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t input_count = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t output_count = PTHREAD_COND_INITIALIZER;
  static size_t input_number = 0;
  static size_t output_number = 0;
  static execution_status status = execution_status::success;

  pthread_mutex_lock (&mutex);
  input_number++;

  if (input_number >= p)
    {
      output_number = 0;
      pthread_cond_broadcast (&input_count);
    }
  else
    while (input_number < p)
      pthread_cond_wait (&input_count, &mutex);

  if (thread_status != execution_status::success)
    status = thread_status;

  output_number++;
  if (output_number >= p)
    {
      input_number = 0;
      pthread_cond_broadcast (&output_count);
    }
  else
    while (output_number < p)
      pthread_cond_wait (&output_count, &mutex);

  bool flag = false;
  if (status != execution_status::success)
    {
      flag = true;
      imp->set_status (status);
    }

  pthread_mutex_unlock (&mutex);
  return flag;
}

void
synchronization_capture_time (size_t p, double *time_value)
{
  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  static pthread_cond_t input_count = PTHREAD_COND_INITIALIZER;
  static pthread_cond_t output_count = PTHREAD_COND_INITIALIZER;
  static size_t input_number = 0;
  static size_t output_number = 0;

  pthread_mutex_lock (&mutex);
  input_number++;
  if (input_number >= p)
    {
      extern double GetTime ();
      *time_value = GetTime ();
      output_number = 0;
      pthread_cond_broadcast (&input_count);
    }
  else
    while (input_number < p)
      pthread_cond_wait (&input_count, &mutex);

  output_number++;
  if (output_number >= p)
    {
      input_number = 0;
      pthread_cond_broadcast (&output_count);
    }
  else
    while (output_number < p)
      pthread_cond_wait (&output_count, &mutex);

  pthread_mutex_unlock (&mutex);
}
