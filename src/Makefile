#-DDEBUG_LOADS or -DPRINT_LOADS
#-DPRINT_LEAF_NUMBER
CC = nvcc
#CFLAGS = -lxml2 -I/usr/include/libxml2 -g -G -O3 -arch=sm_35 -Xcompiler "-std=c99 -g -O3 -fopenmp" #-Wall
#CFLAGS = -lxml2 -I/usr/include/libxml2 -O3 -arch=sm_35 -Xcompiler "-std=c99 -O3 -fopenmp" #-Wall # THETWO
CFLAGS = -lxml2 -I/usr/include/libxml2 -lineinfo -O3 -arch=sm_35 -Xcompiler "-std=c99 -g -O3 -fopenmp" #-Wall # THEONE
#CFLAGS = -lxml2 -I/usr/include/libxml2 -O3 -g -std=c99 -Wall -fopenmp
EXECUTABLE = exec
CFILES = *.c*

all:
	${CC} ${CFLAGS} ${CFILES} -o ../bin/${EXECUTABLE}

clean:
	rm  ../bin/${EXECUTABLE} ../bin/vallog

run:
	pushd ../bin/; ./${EXECUTABLE}; popd

