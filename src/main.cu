/* TODO get tree and leaf array sizes to cut down on GPU memory*/
#include <stdio.h>
#include <string.h>
#include <omp.h>

extern "C" {
  #include "tree.h"
  #include "voxel.h"
  #include "traversal.h"
}
#include "gpu_traversal.h"

//__device__ __constant__ node const_tree[4217];

static float *values;
static short *lindex;

cudaError_t copy_const(node *tree, cudaStream_t stream);
cudaError_t allocate_texture(short *volume);

void gpu_errchk(const char *file, int line, cudaError cuErr) {
  if(cuErr != cudaSuccess)
    printf("EECUDA| Error at %s, %d: %s\n", file, line, cudaGetErrorString(cuErr));
}

void copyTreeToGPU(node *tree, node **gpu_tree, leaf *leaves, leaf **gpu_leaves, int treesize, int leafsize) {
  leaf leafptr;
  //float *values;
  //short *index;

  //printf("2.Host->Device:\n %p->%p\tTree\n%p->%p\tLeaf\n", tree, gpu_tree, leaves,
  //    gpu_leaves);

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) gpu_tree, treesize *
        sizeof(node)));

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) gpu_leaves, sizeof(leaf)));

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) &values, leafsize *
        sizeof(float)));

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) &lindex, leafsize *
        sizeof(short)));

  leafptr.value = values;
  leafptr.index = lindex;

  gpu_errchk(__FILE__, __LINE__, cudaMemcpy(*gpu_leaves, &leafptr, sizeof(leaf),
        cudaMemcpyHostToDevice));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpy(values, leaves->value, leafsize *
        sizeof(float), cudaMemcpyHostToDevice));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpy(lindex, leaves->index, leafsize *
        sizeof(short), cudaMemcpyHostToDevice));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpy(*gpu_tree, tree, treesize *
        sizeof(node), cudaMemcpyHostToDevice));

  //printf("3.Host->Device:\n %p->%p\tTree\n%p->%p\tLeaf\n", tree, gpu_tree, leaves,
  //    gpu_leaves);
}

void copyTreeToGPU2(node *tree, node **gpu_tree, leaf *leaves, leaf **gpu_leaves, int treesize, int leafsize, cudaStream_t *streams) {
  leaf leafptr;
  //float *values;
  //short *index;

  //printf("2.Host->Device:\n %p->%p\tTree\n%p->%p\tLeaf\n", tree, gpu_tree, leaves,
  //    gpu_leaves);

  /*gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) gpu_tree, treesize *
        sizeof(node)));

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) gpu_leaves, sizeof(leaf)));

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) &values, leafsize *
        sizeof(float)));

  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) &index, leafsize *
        sizeof(short)));*/

  leafptr.value = values;
  leafptr.index = lindex;

  gpu_errchk(__FILE__, __LINE__, cudaMemcpyAsync(*gpu_leaves, &leafptr, sizeof(leaf),
        cudaMemcpyHostToDevice, streams[2]));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpyAsync(values, leaves->value, leafsize *
        sizeof(float), cudaMemcpyHostToDevice, streams[3]));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpyAsync(lindex, leaves->index, leafsize *
        sizeof(short), cudaMemcpyHostToDevice, streams[2]));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpyAsync(*gpu_tree, tree, treesize *
        sizeof(node), cudaMemcpyHostToDevice, streams[3]));

  //printf("3.Host->Device:\n %p->%p\tTree\n%p->%p\tLeaf\n", tree, gpu_tree, leaves,
  //    gpu_leaves);
}
int main(int argc, char **argv) {
  short *volume = NULL, *results = NULL;
  char **classes= NULL;
  node *root = NULL;
  leaf *leaves = NULL;
  node *gpu_root = NULL;
  leaf *gpu_leaves = NULL;
  int classCount;
  int x, y, z;
  int treesize = 4217, leafsize = 16384;

  short *gpu_volume = NULL, *gpu_results = NULL;

  double starttime, endtime;

  volume = parseVolume(&x, &y, &z);

  results = allocate_volume(x, y, z);

  cudaDeviceSetCacheConfig(cudaFuncCachePreferL1);



  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) &gpu_volume, x*y*z*sizeof(short)));
  gpu_errchk(__FILE__, __LINE__, cudaMalloc((void**) &gpu_results, x*y*z*sizeof(short)));

  gpu_errchk(__FILE__, __LINE__, cudaMemcpy(gpu_volume, volume, x*y*z*sizeof(short),
      cudaMemcpyHostToDevice));

  printf("DIMS: %d %d %d\n", x, y, z);

  root = parseXMLTree(TREEFILE, &classes, &classCount, &leaves);



  //cudaMemcpyToSymbol(const_tree, root, 4217 *
  //sizeof(node)));
   //   cudaMemcpyHostToDevice));

  //copyTreeToGPU(root, &gpu_root, leaves, &gpu_leaves, treesize, leafsize);

  //starttime = omp_get_wtime();
  //printf("CPU run disabled\n");
  //traverse(root, leaves, volume, results, x, y, z);
  //endtime = omp_get_wtime();

  //printf("Single traversal time, CPU: %f.\n", endtime-starttime);


  /* TODO texture copy */
  //gpu_errchk(__FILE__, __LINE__, allocate_texture(volume));

  dim3 params;
  params.x = 1;
  params.y = 512;
  params.z = 525;
  dim3 params2;
  params2.x = 512;

  cudaStream_t streams[4];

  printf("%d ||||||||\n", sizeof(node));

  for(int repeat = 0; repeat < 4; repeat++)
    cudaStreamCreate(&streams[repeat]);

  gpu_errchk(__FILE__, __LINE__, copy_const(root, streams[0]));

  starttime = omp_get_wtime();
  //gpu_traverse<<<1,1>>>(gpu_root, gpu_leaves, gpu_volume, gpu_results, x, y, z);
  gpu_traverse<<<params,params2,0,streams[0]>>>(gpu_root, gpu_leaves, gpu_volume, gpu_results, x, y, z);
  for(int repeat = 0; repeat < 3; repeat++) {


    //copyTreeToGPU2(root, &gpu_root, leaves, &gpu_leaves, treesize, leafsize, streams);
    cudaDeviceSynchronize();
    gpu_errchk(__FILE__, __LINE__, copy_const(root, streams[0]));
    gpu_traverse<<<params,params2,0,streams[0]>>>(gpu_root, gpu_leaves, gpu_volume, gpu_results, x, y, z);
    gpu_errchk(__FILE__, __LINE__, cudaMemcpyAsync(results, gpu_results,
        x*y*z*sizeof(short), cudaMemcpyDeviceToHost, streams[1]));



  }
  cudaDeviceSynchronize();
  endtime = omp_get_wtime();

  for(int repeat = 0; repeat < 4; repeat++)
    cudaStreamDestroy(streams[repeat]);



  /*Copy results back to host*/
  gpu_errchk(__FILE__, __LINE__, cudaMemcpy(results, gpu_results,
      x*y*z*sizeof(short), cudaMemcpyDeviceToHost));

  printf("Single traversal time, GPU: %f.\n", endtime-starttime);

  if(argc == 3) {
    if(!(strcmp(argv[1], "-csv"))) {
      printf("Printing results to %s.\n", argv[2]);
      toCSV(results, x, y, z, argv[2]);
    }
  }

  gpu_errchk(__FILE__, __LINE__, cudaFree(gpu_volume));
  gpu_errchk(__FILE__, __LINE__, cudaFree(gpu_results));

  freeVolume(volume, y, z);
  freeVolume(results, y, z);
  classes = freeClasses(classes, classCount);

  free(root);
  free(leaves);

  return 0;
}
