#include "test.cuh"

__global__ void test_kernel(void) {
	
}

__global__ void VecAddKernel(float* A, float* B, float* C, int N) {
	int i = threadIdx.x;
	if (i < N) {
		C[i] = A[i] + B[i];
	}
}

namespace Wrapper {
void wrapper(void) {
	test_kernel <<<1, 1>>> ();
	printf("Hello CUDA");
}
void runVecAdd(float* A, float* B, float* C, int N) {
	float* d_A, *d_B, *d_C;

	cudaMalloc((void**)&d_A, N * sizeof(float));
	cudaMalloc((void**)&d_B, N * sizeof(float));
	cudaMalloc((void**)&d_C, N * sizeof(float));

	cudaMemcpy(d_A, A, N * sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, B, N * sizeof(float), cudaMemcpyHostToDevice);

	// Launch the kernel
	VecAddKernel<<<1, N>>>(d_A, d_B, d_C, N);

	cudaMemcpy(C, d_C, N * sizeof(float), cudaMemcpyDeviceToHost);
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);
}
}