#include "tree.h"

#define LEFT 0
#define RIGHT 1

int longRangeContext(short *voxels, char *coords, float threshold, int a, int b, int c, int x, int y, int z);
int coronal(short voxel, int y_center, int z_center, float threshold);
int leafDecision(node *curr, leaf *leaves, int wantClass);
