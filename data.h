#ifndef DATA_H
#define DATA_H

#include "execution_status.h"
#include <stdio.h>
#include <stdlib.h>

class data
{
  double *arr{};
  size_t size{};

public:
  data () = default;
  ~data ();
  data (const data &x) = delete;
  data &operator= (const data &x) = delete;
  data (data &&x) = delete;
  data &operator= (data &&x) = delete;

  execution_status memory_allocate (size_t _size);
  double *get_arr () const;
  size_t get_size () const;
};

#endif // DATA_H
