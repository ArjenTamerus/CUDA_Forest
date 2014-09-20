#include <stdlib.h>
#include <stdio.h>

#include "voxel.h"

/* TODO fix all aborts */

FILE *open_volume() {
  FILE *fp = fopen(VOXEL_DAT, "r");

  if(NULL == fp) {
    printf("EE|| %s:%d: Error opening volume data file.", __FILE__, __LINE__);
    // TODO make nicetynicies
    abort();
  }

  return fp;
}

int read_dims(int *x, int *y, int *z, int *padding, FILE *fp) {
  if(fread(x, sizeof(int), 1, fp) != 1) {
    printf("%s:%d: fread %d\n", __FILE__, __LINE__ - 1, *x);
    abort();
  }
  if(fread(y, sizeof(int), 1, fp) != 1) {
    printf("%s:%d: fread\n", __FILE__, __LINE__ - 1);
    abort();
  }
  if(fread(z, sizeof(int), 1, fp) != 1) {
    printf("%s:%d: fread\n", __FILE__, __LINE__ - 1);
    abort();
  }
  if(fread(&padding, sizeof(int), 1, fp) != 1) {
    printf("%s:%d: fread\n", __FILE__, __LINE__ - 1);
    abort();
  }

  /* NOTE correct for off-by-one in file */
  //*x += 1; *y += 1; *z += 1;
  //*z = 1;

  return 0;
}

short *allocate_volume(int x, int y, int z) {
  short *volume = NULL;
  //int i, j;

  /*
  volume = (short***) malloc(z * sizeof(short**));
  if(NULL == volume) {
    printf("%s:%d: malloc\n", __FILE__, __LINE__);
    abort();
  }

  for(i = 0; i < z; i++) {
    volume[i] = (short**) malloc(y * sizeof(short*));
    if(NULL == volume[i]) {
      printf("%s:%d: malloc\n", __FILE__, __LINE__);
      abort();
    }

    for(j = 0; j < x; j++) {
      volume[i][j] = (short*) malloc(x * sizeof(short));
      if(NULL == volume[i][j]) {
        printf("%s:%d: malloc\n", __FILE__, __LINE__);
        abort();
      }

    }
  } */

  volume = malloc(x*y*z*sizeof(short));

  if(!volume)  {
    printf("%s:%d: malloc\n", __FILE__, __LINE__);
    abort();
  }

  return volume;
}

int read_volume(short *volume, int x, int y, int z, FILE *fp) {
  //int i, j;

  //for(i = 0; i < z; i++) {
    //for(j = 0; j < y; j++) {
      if(fread(volume, sizeof(short), x*y*z, fp) != x*y*z)
        abort();
    //}
  //}

  return 0;
}

int check_dataremains(FILE *fp) {
  int still_remaining = 0;
  short temp;

  while(fread(&temp, sizeof(short), 1, fp)) {
    still_remaining++;
  }

  return still_remaining;
}

short *parseVolume(int *x, int *y, int *z) {
  FILE *fp;
  short *volume = NULL;
  int padding;

  fp = open_volume();

  read_dims(x, y, z, &padding, fp);

  volume = allocate_volume(*x, *y, *z);

  read_volume(volume, *x, *y, *z, fp);

  // XXX temp
  /*int count = check_dataremains(fp);
  if(count)
    printf("Data remains in file! Count: %d\n", count);*/

  fclose(fp);

  return volume;
}

void freeVolume(short *volume, int y, int z) {
  //int i, j;

  /*for(i = 0; i < z; i++) {
    for(j = 0; j < y; j++) {
      free(volume[i][j]);
    }

    free(volume[i]);
  }*/

  free(volume);
}

