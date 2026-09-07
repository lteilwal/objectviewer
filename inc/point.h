/*
 * point.h - 2d and 3d coordinate point structures
 *
 * what this file does:
 * 1. defines point3d for holding 3d coordinates (x, y, z)
 * 2. defines point2d for holding 2d screen coordinates (x, y)
 * 3. adds cuda device compatibility qualifiers so structs can be used in cuda kernels
 */

#pragma once

#ifdef __CUDACC__
#define CUDA_HOST_DEV __host__ __device__
#else
#define CUDA_HOST_DEV
#endif

// 3d point representation
struct Point3D {
	float x, y, z;

	// constructor with coordinates
	CUDA_HOST_DEV Point3D(float px, float py, float pz) {
		x = px;
		y = py;
		z = pz;
	}

	// default constructor initializing to zero
	CUDA_HOST_DEV Point3D() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
};

// 2d point representation for screen coordinates
struct Point2D {
	float x, y;

	// constructor with coordinates
	CUDA_HOST_DEV Point2D(float px, float py) {
		x = px;
		y = py;
	}

	// default constructor initializing to zero
	CUDA_HOST_DEV Point2D() {
		x = 0.0f;
		y = 0.0f;
	}
};