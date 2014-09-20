extern "C" {
  #include <stdio.h>
  #include <stdlib.h>
  #include "tree.h"
}
#include "gpu_node_funcs.h"

__constant__ node const_tree[4217];

__global__ void gpu_traverse(node *root, leaf *leaves, short *voxels, short *results, int x, int y, int z);
