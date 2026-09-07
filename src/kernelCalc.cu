/*
 * kernelCalc.cu - cuda vertex transformation kernel and memory transfer timing
 *
 * what this file does:
 * 1. defines cuda device kernel to transform 3d vertices to 2d screen points in parallel
 * 2. manages gpu device memory allocation and deallocation
 * 3. measures separate timings for host-to-device copy, kernel execution, and device-to-host copy
 * 4. validates cuda execution status and returns transformed 2d points
 */

#include "kernelCalc.h"
#include <cuda_runtime.h>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>

// cuda kernel executing parallel matrix multiplication and screen mapping for each vertex
__global__ void calcPointsKernel(const Point3D* dVerts3D, Point2D* dVerts2D, Mat4 mvp, int w, int h, int n) {
	// calculate global thread index
	int i = blockIdx.x * blockDim.x + threadIdx.x;

	if (i < n) {
		Point3D in = dVerts3D[i];

		// transform vertex by mvp matrix
		float x = mvp.m[0][0] * in.x + mvp.m[0][1] * in.y + mvp.m[0][2] * in.z + mvp.m[0][3];
		float y = mvp.m[1][0] * in.x + mvp.m[1][1] * in.y + mvp.m[1][2] * in.z + mvp.m[1][3];
		float wClip = mvp.m[3][0] * in.x + mvp.m[3][1] * in.y + mvp.m[3][2] * in.z + mvp.m[3][3];

		// check if vertex is in front of camera
		if (wClip > 0.001f) {
			// perspective division to normalized device coordinates
			float ndcX = x / wClip;
			float ndcY = y / wClip;

			// screen viewport mapping
			dVerts2D[i].x = (ndcX + 1.0f) * 0.5f * (float)w;
			dVerts2D[i].y = (1.0f - ndcY) * 0.5f * (float)h;
		}
		else {
			// mark invalid or clipped point
			dVerts2D[i].x = -9999.0f;
			dVerts2D[i].y = -9999.0f;
		}
	}
}

// host function to launch cuda kernel and measure execution stages
int kernelLaunchCalc(const std::vector<Point3D>& verts3D, std::vector<Point2D>& verts2D, const Mat4& mvp, int w, int h, GpuTimings* timings) {
	int vertCount = (int)verts3D.size();
	if (vertCount == 0) {
		return 0;
	}

	// ensure destination buffer size matches vertex count
	verts2D.resize(vertCount);

	// calculate thread block and grid configuration
	int tpb = 256;
	int blocks = (vertCount + tpb - 1) / tpb;

	size_t sizeIn = vertCount * sizeof(Point3D);
	size_t sizeOut = vertCount * sizeof(Point2D);

	Point3D* dVerts3D = nullptr;
	Point2D* dVerts2D = nullptr;

	// allocate gpu device memory
	cudaError_t err = cudaMalloc((void**)&dVerts3D, sizeIn);
	if (err != cudaSuccess) {
		std::cerr << "[CUDA Error] cudaMalloc dVerts3D failed: " << cudaGetErrorString(err) << "\n";
		return 0;
	}

	err = cudaMalloc((void**)&dVerts2D, sizeOut);
	if (err != cudaSuccess) {
		std::cerr << "[CUDA Error] cudaMalloc dVerts2D failed: " << cudaGetErrorString(err) << "\n";
		cudaFree(dVerts3D);
		return 0;
	}

	// measure host to device memory transfer time
	auto h2dStart = std::chrono::high_resolution_clock::now();
	err = cudaMemcpy(dVerts3D, verts3D.data(), sizeIn, cudaMemcpyHostToDevice);
	auto h2dEnd = std::chrono::high_resolution_clock::now();
	float h2dMs = std::chrono::duration<float, std::milli>(h2dEnd - h2dStart).count();
	if (err != cudaSuccess) {
		std::cerr << "[CUDA Error] cudaMemcpy HtoD failed: " << cudaGetErrorString(err) << "\n";
	}

	// measure cuda kernel execution time
	auto kernelStart = std::chrono::high_resolution_clock::now();
	calcPointsKernel<<<blocks, tpb>>>(dVerts3D, dVerts2D, mvp, w, h, vertCount);
	err = cudaGetLastError();
	if (err != cudaSuccess) {
		std::cerr << "[CUDA Error] Kernel launch failed: " << cudaGetErrorString(err) << "\n";
	}
	err = cudaDeviceSynchronize();
	if (err != cudaSuccess) {
		std::cerr << "[CUDA Error] cudaDeviceSynchronize failed: " << cudaGetErrorString(err) << "\n";
	}
	auto kernelEnd = std::chrono::high_resolution_clock::now();
	float kernelMs = std::chrono::duration<float, std::milli>(kernelEnd - kernelStart).count();

	// measure device to host memory transfer time
	auto d2hStart = std::chrono::high_resolution_clock::now();
	err = cudaMemcpy(verts2D.data(), dVerts2D, sizeOut, cudaMemcpyDeviceToHost);
	auto d2hEnd = std::chrono::high_resolution_clock::now();
	float d2hMs = std::chrono::duration<float, std::milli>(d2hEnd - d2hStart).count();
	if (err != cudaSuccess) {
		std::cerr << "[CUDA Error] cudaMemcpy DtoH failed: " << cudaGetErrorString(err) << "\n";
	}

	// free gpu device memory
	cudaFree(dVerts3D);
	cudaFree(dVerts2D);

	// store measured timings if pointer was provided
	if (timings != nullptr) {
		timings->h2dMs = h2dMs;
		timings->kernelMs = kernelMs;
		timings->d2hMs = d2hMs;
		timings->totalMs = h2dMs + kernelMs + d2hMs;
	}

	return 1;
}