#include <float.h>

#include "gpu_node_funcs.h"
#include "gpu_traversal.h"
extern "C" {
  #include "voxel.h"
}
void gpu_errchk(const char *file, int line, cudaError cuErr);

#define test(func) gpu_errchk(__FILE__,__LINE__,func)

//__device__ __constant__ node const_tree[4217];
texture<short, 3, cudaReadModeElementType> tex;
cudaArray *volume_array;

__device__ int gpu_longRangeContext(short *voxels, char *coords, float threshold, int a, int b, int c, int x, int y, int z) {
  //int pos = (c * y * x) + (b * x) + a;
  if(voxels[a] < threshold)
    return LEFT;
  return RIGHT;
}

// TODO
__device__ int gpu_coronal(short voxel, int y_center, int z_center, float threshold) {
  if((voxel - y_center) < threshold)
    return LEFT;
  return RIGHT;
}

#define treenode const_tree[cur]
__global__ /*__launch_bounds__(512, 4)*/ void gpu_traverse(node *tree, leaf *leaves, short *voxels, short *results, int x, int y, int z) {

  node *curr;
  int cur;
  int i, j, k;
  int a, b, c;
  int pos, alt_pos;
  char res;
  int cookie;

  /* XXX i-j-k = z-y-x */

  i = blockIdx.z;
  /*if(blockIdx.y & 3 == 1) {
    j = (blockIdx.y / 4);
    k = threadIdx.x + 128;
  } else if(blockIdx.y & 3 == 2) {
    j = (blockIdx.y / 4);
    k = threadIdx.x + 256;
  } else if(blockIdx.y & 3 == 3) {
    j = (blockIdx.y / 4);
    k = threadIdx.x + 384;
  } else {
    j = (blockIdx.y / 4);
    k = threadIdx.x;
  }*/
  j = blockIdx.y;
  k = threadIdx.x;


  cur = 0;

  pos = (i * x * y) + (j * x) + k;
  while(1) {
    if(treenode.type == LONGRANGECONTEXT) {
      a = (k + treenode.arguments[0]) & (x-1);
      b = (j + treenode.arguments[1]) & (x-1);
      c = i + treenode.arguments[2];

      if(c < 0)
        c += z;
      else if (c >= z)
        c -= z;

      alt_pos = (c * y * x) + (b * x) + a;
      res = voxels[alt_pos] < treenode.threshold ? LEFT : RIGHT;
      cur = treenode.children[res];
    }
    else if(treenode.type == CORONAL) {
      res = (x - (y>>1)) < treenode.threshold ? LEFT : RIGHT;
      cur = treenode.children[res];
    }
    else {
      break;
    }
  }

  results[pos] = treenode.arguments[0]; //leaf number/ID

}

cudaError_t copy_const(node *tree, cudaStream_t stream) {
  //printf("Host root type: %d\n", tree[0].type);
  return cudaMemcpyToSymbolAsync(const_tree, tree, 4217 * sizeof(node), 0,
  cudaMemcpyHostToDevice, stream);
}

cudaError_t allocate_texture(short *volume) {
  tex.addressMode[0] = cudaAddressModeWrap;
  tex.addressMode[1] = cudaAddressModeWrap;
  tex.addressMode[2] = cudaAddressModeWrap;
  tex.filterMode = cudaFilterModeLinear;
  tex.normalized = false;

  cudaChannelFormatDesc format = {16, 0, 0, 0, cudaChannelFormatKindSigned};

  cudaExtent extent = {512, 512, 525};

  test(cudaMalloc3DArray(&volume_array, &format, extent, 0));

  cudaMemcpy3DParms params = {0};

  int pitch = sizeof(float);

  params.srcPtr = make_cudaPitchedPtr((void*) volume, pitch, 512*512*525, 1);
  params.dstArray = volume_array;
  params.extent = extent;
  params.kind = cudaMemcpyHostToDevice;

  test(cudaMemcpy3D(&params));


  //test(cudaMemcpyToArray(volume_array, 0, 0, volume, 512*512*525, cudaMemcpyHostToDevice));

  return cudaBindTextureToArray(tex, volume_array, format);
}
