#include "mesh_index.h"

int mesh_index::ij_to_l (int nx, int i, int j)
{
  return i + j * (nx + 1);
}

void mesh_index::l_to_ij (int nx, int l, int &i, int &j)
{
  j = l / (nx + 1);
  i = l - j * (nx + 1);
}

void mesh_index::thread_range (int n, int p, int k, int &begin, int &end)
{
  begin = n * k / p;
  end = n * (k + 1) / p;
}
