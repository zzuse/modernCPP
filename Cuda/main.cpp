// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include "common.h"
#include <stdio.h>

__global__ void hello_cuda() { printf("Hello CUDA world\n"); }

int showDim()
{
    int nx, ny;
    nx = 16;
    ny = 4;
    // dims3 is a CUDA-provided structure used to specify dimensions in up to three dimensions (x, y, z) for thread
    // blocks and grids, if you only provide one value, it sets x and leaves y and z as 1.
    dim3 block(8, 2);
    dim3 grid(nx / block.x, ny / block.y); // grid.x = 2, grid.y = 2, grid.z = 1

    // async running on kernel, The total number of threads launched is grid * block
    // grid: Specifies the number of blocks in the grid (how many groups of threads to launch).
    // block: Specifies the number of threads per block (how many threads in each group).
    hello_cuda<<<grid, block>>>();
    // wait kernel
    cudaDeviceSyncronize();

    cudaDeviceReset();
    return 0;
}

int main() { showDim(); }