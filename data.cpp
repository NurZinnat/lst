#include "data.h"

data::~data ()
{
  if (arr)
    delete[] arr;
}

execution_status
data::memory_allocate (size_t _size)
{
  if (size != 0)
    {
      printf ("data::memory_allocate: повторное выделение памяти\n");
      return execution_status::runtime;
    }
  size = _size;
  arr = new double[size];
  if (!arr)
    {
      printf ("data::memory_allocate: не удалось выделить память: double, %ld\n",
              size);
      return execution_status::memory;
    }
  return execution_status::success;
}

double *
data::get_arr () const
{
  if (!arr)
    printf ("data::get_arr: arr == nullptr\n");
  return arr;
}

size_t
data::get_size () const
{
  return size;
}
