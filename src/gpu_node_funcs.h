#include "tree.h"

#define LEFT 0
#define RIGHT 1

__device__ int gpu_longRangeContext(short *voxels, short *coords, float threshold, int a, int b, int c, int x, int y, int z);
__device__ int gpu_coronal(short voxel, int y_center, int z_center, float threshold);
__device__ int gpu_leafDecision(node *curr, leaf *leaves, int wantClass);
