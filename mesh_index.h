#ifndef MESH_INDEX_H
#define MESH_INDEX_H

class mesh_index {
 public:
  static int ij_to_l (int nx, int i, int j);
  static void l_to_ij (int nx, int l, int &i, int &j);
  static void thread_range (int n, int p, int k, int &begin, int &end);
};

#endif // MESH_INDEX_H
