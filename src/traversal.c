#include "traversal.h"
#include "voxel.h"
//#include <omp.h>
#include <float.h>

#include "node_funcs.h"
#include <omp.h>

inline int longRangeContext(short *voxels, char *coords, float threshold, int a, int b, int c, int x, int y, int z) {
  //int pos = (c * y * x) + (b * x) + a;
  if(voxels[a] < threshold)
    return LEFT;
  return RIGHT;
}

  inline int coronal(short voxel, int y_center, int z_center, float threshold) {
    if((voxel - y_center) < threshold)
      return LEFT;
    return RIGHT;
  }

/*inline int leafDecision(node *curr, leaf *leaves, int wantClass) {
  int start = curr->arguments[1];
  int end = start + curr->arguments[2];

  int i;
  int largest_index = start;

  if(wantClass) {
  for(i = start; i < end; i++) {
  if(leaves->value[i] > leaves->value[largest_index])
  largest_index = i;
  }

  return leaves->index[largest_index];
  }

  return curr->arguments[0];
  }*/

#define this tree[cur]
void traverse(node *tree, leaf *leaves, short *voxels, short *results, int x, int y, int z) {
  node *curr;// = root;
  int cur = 0;
  int i, j, k;
  int a, b, c;
  int pos, alt_pos;
  char res;
  int cookie;

  //#pragma omp parallel for schedule(dynamic) default(none) private(j,k,cur,a,b,c,pos,alt_pos,res) shared(tree,voxels,results,x,y,z)
  for(i = 0; i < z; i++) {
    //if(i == 0 && omp_get_thread_num() == 0)
      //printf("OMP %d\n", omp_get_num_threads());
    for(j = 0; j < y; j++) {
      for(k = 0; k < x; k++) {
        pos = (i * x * y) + (j * x) + k;
        cur = 0;
        while(1) {
          if(this.type == LONGRANGECONTEXT) {
            a = (x + k + this.arguments[0]) & (x-1);
            b = (y + j + this.arguments[1]) & (y-1);
            c = i + this.arguments[2];

            if(c < 0)
              c += z;
            else if(c > z-1)
              c -= z;

            alt_pos = (c * y * x) + (b * x) + a;
            res = voxels[alt_pos] < this.threshold ? LEFT : RIGHT;
            cur = this.children[res];
          }
          else if(this.type == CORONAL) {
            res = (x - (y>>1)) < this.threshold ? LEFT : RIGHT;
            cur = this.children[res];
          }
          else {
            results[pos] = this.arguments[0];//leafDecision(curr, leaves, 1);
            break;
          }
        }
      }
    }
  }

}

void toCSV(short *voxels, int x, int y, int z, const char *filename) {
  FILE *fp;
  int i, j, k, pos;

  fp = fopen(filename, "w");

  fprintf(fp, "x,y,z,class\n");

  /*for(i = 0; i < z; i++) {
    for(j = 0; j < y; j++) {
    for(k = 0; k < x; k++) {
    if(voxels[i][j][k]) {
    fprintf(fp, "%d,%d,%d,%d\n", k, j, i, voxels[i][j][k]);
    }
    }
    }
    }*/

  printf("Printing: %d, %d, %d\n", x, y, z);

  for(i = 0; i < z; i++) {
    for(j = 0; j < y; j++) {
      for(k = 0; k < x; k++) {
        pos = (i * y * x) + (j * x) + k;
        if(voxels[pos])
          fprintf(fp, "%d,%d,%d,%d\n", k, j, i, voxels[pos]);
      }
    }
  }

  fclose(fp);
}

