#pragma once

#include <cuda_runtime.h>
#include <cuda.h>
#include <device_launch_parameters.h>

#include <stdio.h>

namespace Wrapper {
void wrapper(void);
void runVecAdd(float* A, float* B, float* C, int N);
}