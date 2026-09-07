/*
 * kernelCalc.h - cuda kernel launcher and gpu profiling declarations
 *
 * what this file does:
 * 1. declares gpu vertex transformation kernel launch function
 * 2. passes mvp matrix and screen dimensions to device
 * 3. records separate timings for host-to-device copy, kernel execution, and device-to-host copy
 */

#pragma once
#include <vector>
#include "point.h"
#include "calc.h"

// structure to hold cuda timing measurements in milliseconds
struct GpuTimings {
	float h2dMs = 0.0f;
	float kernelMs = 0.0f;
	float d2hMs = 0.0f;
	float totalMs = 0.0f;
};

// launches cuda kernel to transform 3d vertices to 2d screen points with timing
int kernelLaunchCalc(const std::vector<Point3D>& verts3D, std::vector<Point2D>& verts2D, const Mat4& mvp, int w, int h, GpuTimings* timings = nullptr);