#include "thread_object.h"

#include "implementation.h"
#include "synch.h"

void *
worker_thread (void *arg)
{
  thread_object *curr = static_cast<thread_object *> (arg);
  curr->GO ();
  return nullptr;
}

execution_status
thread_object::init_thread_object (size_t _thread_index, implementation *_IMP)
{
  thread_index = _thread_index;
  IMP = _IMP;
  p = IMP->get_p ();

  return init_fem_core (IMP->get_domain (), IMP->get_function_index (),
                       IMP->get_tolerance (), IMP->get_max_iterations (), p,
                       IMP->get_msr_index (), IMP->get_msr_values (),
                       IMP->get_msr_length (), IMP->get_rhs (), IMP->get_solution (),
                       IMP->get_residual (), IMP->get_search (), IMP->get_auxiliary ());
}

void
thread_object::GO ()
{
  worker_build_system (thread_index);
  if (synchronization_error_check (p, get_status (), IMP))
    return;

  worker_compute_errors (thread_index);
  synchronization_error_check (p, get_status (), IMP);
}
