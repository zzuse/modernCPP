// INSTALL: sudo port install gcc13
// COMPILE: /opt/local/bin/g++-mp-13 -std=c++20 -fcoroutines -pthread -o main main.cpp
// jthread only supported by gcc
#include "common.h"
#include <stdio.h>
#include <time.h>

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

__global__ void unique_idx_calc_threadIdx(int* input)
{
    int tid = threadIdx.x;
    printf("threadIdx : %d, value : %d \n", tid, input[tid]);
}

int showThreadidx()
{
    int array_size = 8;
    int array_byte_size = sizeof(int) * array_size;
    int h_data[] = {23, 9, 4, 53, 65, 12, 1, 33};

    for (int i = 0; i < array_size; i++) {
        printf("%d ", h_data[i]);
    }
    printf("\n \n");

    int* d_data;
    cudaMalloc((void**)&d_data, array_byte_size);
    cudaMemcpy(d_data, h_data, array_byte_size, cudaMemcpyHostToDevice);

    dim3 block(8); // 8 piece of data access by 8 thread
    dim3 gird(1);

    unique_idx_calc_threadIdx<<<grid, block>>>(d_data);
    cudaDeviceSyncronize();

    dim3 block(4); // first half piece of data access by 8 thread
    dim3 gird(2);

    cudaDeviceReset();
    return 0;
}

void cputime()
{
    // CPU time
    clock_t cpu_start, cpu_end;
    cpu_start = clock();
    // do some cpu thing;
    cpu_end = clock();
    printf("Sum CPU execution time: %4.6f \n", (double)((double)(cpu_end - cpu_start) / CLOCKS_PER_SEC));
}

void gputime()
{
    clock_t htod_start, htod_end;
    htod_start = clock();
    // cudaMemcpy(,,,cudaMemcpyHostToDevice);
    htod_end = clock();

    clock_t gpu_start, gpu_end;
    gpu_start = clock();
    // do some gpu thing;
    cudaDeviceSynchronize();
    gpu_end = clock();

    clock_t dtoh_start, dtoh_end;
    dtoh_start = clock();
    // cudaMemcpy(, , , cudaMemcpyDeviceToHost);
    dtoh_end = clock();

    printf("Sum GPU execution time: %4.6f \n", (double)((double)(dtoh_end - htod_start) / CLOCKS_PER_SEC));
}

int main()
{
    showDim();
    showThreadidx();
}
