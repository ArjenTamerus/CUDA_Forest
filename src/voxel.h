#define VOXEL_DAT "../../data/volume.dat"

short *parseVolume(int *x, int *y, int *z); //int *padding?
short *allocate_volume(int x, int y, int z);
void freeVolume(short *volume, int y, int z);
