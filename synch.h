#ifndef SYNCH_H
#define SYNCH_H

#include "execution_status.h"
#include <stddef.h>

class implementation;

void synchronization_partial_bind (double *buffer, size_t p);
void synchronization_barrier (size_t p);
void synchronization_reduce_int (size_t p, int *value, size_t length);
void synchronization_partial_store (size_t p, size_t thread_index, double value);
double synchronization_partial_sum (size_t p);
double synchronization_partial_max (size_t p);
bool synchronization_error_check (size_t p, execution_status thread_status,
                                  implementation *imp);
void synchronization_capture_time (size_t p, double *time_value);

#endif // SYNCH_H
