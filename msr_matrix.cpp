#include "msr_matrix.h"

#include "domain.h"
#include "mesh_index.h"
#include "synch.h"

static int fill_entry (int nx, int ny, double hx, double hy, int i, int j, int is, int js,
                       int storage_index, int *index, double *values)
{
  const int center = mesh_index::ij_to_l (nx, i, j);
  const int neighbor = mesh_index::ij_to_l (nx, is, js);

  if (index != nullptr)
    index[storage_index] = neighbor;

  if (values == nullptr)
    return 1;

  if (center == neighbor)
    {
      values[storage_index]
          = ((i < nx && j > 0 ? 1 : 0) + (i > 0 && j > 0 ? 2 : 0)
             + (i > 0 && j < ny ? 1 : 0) + (i < nx && j < ny ? 2 : 0))
            * (hx * hy / 12.);
      return 0;
    }

  values[storage_index]
      = (static_cast<int> (is == i + 1 && js == j)
             ? (static_cast<int> (j < ny) + static_cast<int> (j > 0))
             : 0)
        + (static_cast<int> (is == i && js == j - 1)
               ? (static_cast<int> (i < nx) + static_cast<int> (i > 0))
               : 0)
        + (static_cast<int> (is == i - 1 && js == j - 1) ? 2 : 0)
        + (static_cast<int> (is == i - 1 && js == j)
               ? (static_cast<int> (j > 0) + static_cast<int> (j < ny))
               : 0)
        + (static_cast<int> (is == i && js == j + 1)
               ? (static_cast<int> (i > 0) + static_cast<int> (i < nx))
               : 0)
        + (static_cast<int> (is == i + 1 && js == j + 1) ? 2 : 0);
  values[storage_index] *= hx * hy / 24.;

  return 0;
}

void msr_matrix::attach (int *index, double *storage, int length, int node_count)
{
  index_ = index;
  storage_ = storage;
  length_ = length;
  node_count_ = node_count;
}

int msr_matrix::nonzero_estimate (int nx, int ny)
{
  return (nx - 1) * (ny - 1) * 6 + 4 * (2 * (nx - 1) + 2 * (ny - 1)) + 2 * 3 + 2 * 2;
}

int msr_matrix::count_off_diagonal (int nx, int ny, int i, int j)
{
  int count = 0;

  if (i > 0 && j > 0)
    ++count;
  if (j > 0)
    ++count;
  if (i > 0)
    ++count;
  if (i < nx)
    ++count;
  if (j < ny)
    ++count;
  if (i < nx && j < ny)
    ++count;

  return count;
}

int msr_matrix::build_index_pattern (int nx, int ny, int *index)
{
  const int node_count = (nx + 1) * (ny + 1);
  int cursor = node_count + 1;

  for (int l = 0; l < node_count; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);
      const int neighbors = count_off_diagonal (nx, ny, i, j);
      index[l] = cursor;
      cursor += neighbors;
    }

  index[node_count] = cursor;
  return cursor;
}

int msr_matrix::fill_diagonal_entry (int nx, int ny, double hx, double hy, int i, int j,
                                    double *value) const
{
  const int position = mesh_index::ij_to_l (nx, i, j);
  return fill_entry (nx, ny, hx, hy, i, j, i, j, position, nullptr, value);
}

int msr_matrix::fill_off_diagonal_entries (int nx, int ny, double hx, double hy, int i,
                                          int j, int *index, double *values) const
{
  int count = 0;

  if (i > 0 && j > 0)
    fill_entry (nx, ny, hx, hy, i, j, i - 1, j - 1, count++, index, values);
  if (j > 0)
    fill_entry (nx, ny, hx, hy, i, j, i, j - 1, count++, index, values);
  if (i > 0)
    fill_entry (nx, ny, hx, hy, i, j, i - 1, j, count++, index, values);
  if (i < nx)
    fill_entry (nx, ny, hx, hy, i, j, i + 1, j, count++, index, values);
  if (j < ny)
    fill_entry (nx, ny, hx, hy, i, j, i, j + 1, count++, index, values);
  if (i < nx && j < ny)
    fill_entry (nx, ny, hx, hy, i, j, i + 1, j + 1, count++, index, values);

  return count;
}

int msr_matrix::fill_parallel (const domain &domain, int thread_count, int thread_id)
{
  const int nx = domain.nx ();
  const int ny = domain.ny ();
  const int node_count = domain.node_count ();
  const double hx = domain.hx ();
  const double hy = domain.hy ();

  int begin = 0;
  int end = 0;
  int error = 0;
  int off_diagonal_count = 0;

  mesh_index::thread_range (node_count, thread_count, thread_id, begin, end);

  for (int l = begin; l < end; ++l)
    {
      int i = 0;
      int j = 0;
      mesh_index::l_to_ij (nx, l, i, j);

      if (fill_diagonal_entry (nx, ny, hx, hy, i, j, storage_) != 0)
        {
          error = -1;
          break;
        }

      const int row_start = index_[l];
      const int row_length = index_[l + 1] - row_start;
      const int written = fill_off_diagonal_entries (nx, ny, hx, hy, i, j, index_ + row_start,
                                                     storage_ + row_start);
      if (row_length != written)
        {
          error = -2;
          break;
        }
      off_diagonal_count += written;
    }

  synchronization_reduce_int (thread_count, &error, 1);
  if (error < 0)
    return -1;

  synchronization_reduce_int (thread_count, &off_diagonal_count, 1);
  if (index_[node_count] != node_count + 1 + off_diagonal_count)
    return -2;

  return 0;
}

int msr_matrix::finalize_pattern (int nx, int ny)
{
  return build_index_pattern (nx, ny, index_);
}
