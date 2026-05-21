#ifndef THREAD_OBJECT_H
#define THREAD_OBJECT_H

#include "execution_status.h"
#include "fem_core.h"
#include <pthread.h>

class implementation;

void *worker_thread (void *arg);

class thread_object : public fem_core
{
  size_t thread_index = 0;
  size_t p = 0;
  implementation *IMP{};

public:
  thread_object () = default;
  ~thread_object () = default;
  thread_object (const thread_object &x) = delete;
  thread_object &operator= (const thread_object &x) = delete;
  thread_object (thread_object &&x) = delete;
  thread_object &operator= (thread_object &&x) = delete;

  execution_status init_thread_object (size_t _thread_index, implementation *_IMP);
  void GO ();
};

#endif // THREAD_OBJECT_H
