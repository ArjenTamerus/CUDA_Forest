#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include "node_funcs.h"

void traverse(node *root, leaf *leaves, short *voxels, short *results, int x, int y, int z);
void toCSV(short *voxels, int x, int y, int z, const char *filename);
