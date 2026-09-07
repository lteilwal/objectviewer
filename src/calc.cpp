/*
 * calc.cpp - matrix transformations, cpu vertex processing, and benchmark suite
 *
 * what this file does:
 * 1. implements 4x4 matrix operations (multiply, translate, rotate, scale, perspective)
 * 2. calculates model and view matrices for object and camera
 * 3. transforms 3d vertices on cpu with perspective division and screen mapping
 * 4. runs comparative cpu vs cuda benchmarks across multiple workload sizes
 * 5. calculates speedup ratios and prints formatted benchmark results to console
 */

#include "calc.h"
#include "mesh.h"
#include "renderdata.h"
#include "kernelCalc.h"
#include <cmath>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <string>

static const float PI = 3.14159265359f;

// create identity matrix
Mat4 Mat4::identity() {
	Mat4 res;
	for (int i = 0; i < 4; i++) {
		res.m[i][i] = 1.0f;
	}
	return res;
}

// multiply two 4x4 matrices
Mat4 Mat4::multiply(const Mat4& a, const Mat4& b) {
	Mat4 res;
	for (int row = 0; row < 4; row++) {
		for (int col = 0; col < 4; col++) {
			res.m[row][col] = 0.0f;
			for (int k = 0; k < 4; k++) {
				res.m[row][col] += a.m[row][k] * b.m[k][col];
			}
		}
	}
	return res;
}

// create translation matrix
Mat4 Mat4::makeTranslation(float tx, float ty, float tz) {
	Mat4 res = identity();
	res.m[0][3] = tx;
	res.m[1][3] = ty;
	res.m[2][3] = tz;
	return res;
}

// create scale matrix
Mat4 Mat4::makeScale(float sx, float sy, float sz) {
	Mat4 res = identity();
	res.m[0][0] = sx;
	res.m[1][1] = sy;
	res.m[2][2] = sz;
	return res;
}

// create rotation matrix around x-axis
Mat4 Mat4::makeRotateX(float angleDeg) {
	Mat4 res = identity();
	float rad = angleDeg * (PI / 180.0f);
	float c = cosf(rad);
	float s = sinf(rad);
	res.m[1][1] = c;
	res.m[1][2] = -s;
	res.m[2][1] = s;
	res.m[2][2] = c;
	return res;
}

// create rotation matrix around y-axis
Mat4 Mat4::makeRotateY(float angleDeg) {
	Mat4 res = identity();
	float rad = angleDeg * (PI / 180.0f);
	float c = cosf(rad);
	float s = sinf(rad);
	res.m[0][0] = c;
	res.m[0][2] = s;
	res.m[2][0] = -s;
	res.m[2][2] = c;
	return res;
}

// create rotation matrix around z-axis
Mat4 Mat4::makeRotateZ(float angleDeg) {
	Mat4 res = identity();
	float rad = angleDeg * (PI / 180.0f);
	float c = cosf(rad);
	float s = sinf(rad);
	res.m[0][0] = c;
	res.m[0][1] = -s;
	res.m[1][0] = s;
	res.m[1][1] = c;
	return res;
}

// create perspective projection matrix
Mat4 Mat4::makePerspective(float fovDeg, float aspect, float nearZ, float farZ) {
	Mat4 res;
	float rad = (fovDeg * 0.5f) * (PI / 180.0f);
	float tanHalfFov = tanf(rad);
	res.m[0][0] = 1.0f / (aspect * tanHalfFov);
	res.m[1][1] = 1.0f / tanHalfFov;
	res.m[2][2] = -(farZ + nearZ) / (farZ - nearZ);
	res.m[2][3] = -(2.0f * farZ * nearZ) / (farZ - nearZ);
	res.m[3][2] = -1.0f;
	return res;
}

// compute model matrix from position, rotation, and scale
Mat4 Transform::getModelMatrix() const {
	Mat4 s = Mat4::makeScale(scale.x, scale.y, scale.z);
	Mat4 rx = Mat4::makeRotateX(rotation.x);
	Mat4 ry = Mat4::makeRotateY(rotation.y);
	Mat4 rz = Mat4::makeRotateZ(rotation.z);
	Mat4 t = Mat4::makeTranslation(position.x, position.y, position.z);

	// combine transformations in order: scale, rotate, translate
	Mat4 rot = Mat4::multiply(rz, Mat4::multiply(ry, rx));
	Mat4 model = Mat4::multiply(t, Mat4::multiply(rot, s));
	return model;
}

// compute view matrix from camera orbit parameters
Mat4 Camera::getViewMatrix() const {
	Mat4 tTarget = Mat4::makeTranslation(-target.x, -target.y, -target.z);
	Mat4 rYaw = Mat4::makeRotateY(-yaw);
	Mat4 rPitch = Mat4::makeRotateX(-pitch);
	Mat4 tDist = Mat4::makeTranslation(0.0f, 0.0f, -distance);

	// combine camera transformations
	Mat4 view = Mat4::multiply(tDist, Mat4::multiply(rPitch, Mat4::multiply(rYaw, tTarget)));
	return view;
}

// transform a single 3d point using matrix
bool transformPoint(const Mat4& mat, const Point3D& in, Point3D& out) {
	float x = mat.m[0][0] * in.x + mat.m[0][1] * in.y + mat.m[0][2] * in.z + mat.m[0][3];
	float y = mat.m[1][0] * in.x + mat.m[1][1] * in.y + mat.m[1][2] * in.z + mat.m[1][3];
	float z = mat.m[2][0] * in.x + mat.m[2][1] * in.y + mat.m[2][2] * in.z + mat.m[2][3];
	float w = mat.m[3][0] * in.x + mat.m[3][1] * in.y + mat.m[3][2] * in.z + mat.m[3][3];

	// discard points behind camera
	if (w <= 0.001f) {
		return false;
	}

	out.x = x / w;
	out.y = y / w;
	out.z = z / w;
	return true;
}

// map normalized device coordinates to screen pixels
Point2D screenMap(const Point3D& ndc, int w, int h) {
	Point2D coord;
	coord.x = (ndc.x + 1.0f) * 0.5f * (float)w;
	coord.y = (1.0f - ndc.y) * 0.5f * (float)h;
	return coord;
}

// transform vertex buffer on cpu and measure execution time
float transformVerticesCPU(const std::vector<Point3D>& verts3D, std::vector<Point2D>& verts2D, const Mat4& mvp, int w, int h) {
	size_t count = verts3D.size();
	verts2D.resize(count);

	// record start timestamp for cpu execution
	auto start = std::chrono::high_resolution_clock::now();

	// iterate over vertices and apply mvp transformation
	for (size_t i = 0; i < count; i++) {
		const Point3D& in = verts3D[i];
		float x = mvp.m[0][0] * in.x + mvp.m[0][1] * in.y + mvp.m[0][2] * in.z + mvp.m[0][3];
		float y = mvp.m[1][0] * in.x + mvp.m[1][1] * in.y + mvp.m[1][2] * in.z + mvp.m[1][3];
		float z = mvp.m[2][0] * in.x + mvp.m[2][1] * in.y + mvp.m[2][2] * in.z + mvp.m[2][3];
		float wClip = mvp.m[3][0] * in.x + mvp.m[3][1] * in.y + mvp.m[3][2] * in.z + mvp.m[3][3];

		if (wClip > 0.001f) {
			float ndcX = x / wClip;
			float ndcY = y / wClip;
			verts2D[i].x = (ndcX + 1.0f) * 0.5f * (float)w;
			verts2D[i].y = (1.0f - ndcY) * 0.5f * (float)h;
		}
		else {
			verts2D[i].x = -9999.0f;
			verts2D[i].y = -9999.0f;
		}
	}

	// record end timestamp for cpu execution
	auto end = std::chrono::high_resolution_clock::now();
	return std::chrono::duration<float, std::milli>(end - start).count();
}

// benchmark helper function for a specific vertex list
static void benchmarkWorkload(const std::string& label, const std::vector<Point3D>& verts, const Mat4& mvp, int w, int h, int iterations) {
	std::vector<Point2D> cpuOut;
	std::vector<Point2D> gpuOut;

	// warm-up iterations
	for (int i = 0; i < 5; i++) {
		transformVerticesCPU(verts, cpuOut, mvp, w, h);
		kernelLaunchCalc(verts, gpuOut, mvp, w, h, nullptr);
	}

	float totalCpuMs = 0.0f;
	float totalH2DMs = 0.0f;
	float totalKernelMs = 0.0f;
	float totalD2HMs = 0.0f;

	// run timed benchmark iterations
	for (int i = 0; i < iterations; i++) {
		totalCpuMs += transformVerticesCPU(verts, cpuOut, mvp, w, h);

		GpuTimings timings;
		kernelLaunchCalc(verts, gpuOut, mvp, w, h, &timings);
		totalH2DMs += timings.h2dMs;
		totalKernelMs += timings.kernelMs;
		totalD2HMs += timings.d2hMs;
	}

	// calculate average timings per iteration
	float avgCpu = totalCpuMs / (float)iterations;
	float avgH2D = totalH2DMs / (float)iterations;
	float avgKernel = totalKernelMs / (float)iterations;
	float avgD2H = totalD2HMs / (float)iterations;
	float avgGpuTotal = avgH2D + avgKernel + avgD2H;

	// calculate speedup ratios
	float totalSpeedup = (avgGpuTotal > 0.0f) ? (avgCpu / avgGpuTotal) : 0.0f;
	float kernelSpeedup = (avgKernel > 0.0f) ? (avgCpu / avgKernel) : 0.0f;

	// print row of benchmark results
	std::cout << std::left << std::setw(20) << label
		<< std::right << std::setw(10) << verts.size()
		<< std::setw(15) << std::fixed << std::setprecision(3) << avgCpu
		<< std::setw(18) << std::fixed << std::setprecision(3) << avgGpuTotal
		<< std::setw(13) << std::fixed << std::setprecision(3) << avgH2D
		<< std::setw(15) << std::fixed << std::setprecision(3) << avgKernel
		<< std::setw(13) << std::fixed << std::setprecision(3) << avgD2H
		<< std::setw(17) << std::fixed << std::setprecision(2) << kernelSpeedup << "x"
		<< std::setw(16) << std::fixed << std::setprecision(2) << totalSpeedup << "x"
		<< "\n";
}

// execute full benchmark suite on loaded mesh and scaled workloads
void runBenchmarkSuite(const Mesh& mesh, const rData& rd, int iterations) {
	// calculate current model view projection matrix
	Mat4 model = rd.modelTransform.getModelMatrix();
	Mat4 view = rd.camera.getViewMatrix();
	float aspect = (float)rd.w / (float)rd.h;
	Mat4 proj = Mat4::makePerspective(60.0f, aspect, 0.1f, 500.0f);
	Mat4 mvp = Mat4::multiply(proj, Mat4::multiply(view, model));

	std::cout << "\n========================================================================================================\n";
	std::cout << "                             CPU vs CUDA BENCHMARK & PERFORMANCE PROFILE                                \n";
	std::cout << "========================================================================================================\n";
	std::cout << "iterations per test: " << iterations << " | window size: " << rd.w << "x" << rd.h << "\n";
	std::cout << "loaded mesh vertex count: " << mesh.verts.size() << " | edge count: " << mesh.edges.size() << "\n\n";

	// print table header
	std::cout << std::left << std::setw(20) << "workload"
		<< std::right << std::setw(10) << "vertices"
		<< std::setw(15) << "CPU (ms)"
		<< std::setw(18) << "GPU total (ms)"
		<< std::setw(13) << "H->D (ms)"
		<< std::setw(15) << "Kernel (ms)"
		<< std::setw(13) << "D->H (ms)"
		<< std::setw(17) << "Kernel speedup"
		<< std::setw(16) << "Total speedup"
		<< "\n";
	std::cout << std::string(137, '-') << "\n";

	// benchmark loaded mesh
	if (!mesh.verts.empty()) {
		benchmarkWorkload("loaded mesh", mesh.verts, mvp, rd.w, rd.h, iterations);
	}

	// test multiple synthetic workload sizes for scaling analysis
	std::vector<int> workloadSizes = {
	1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000,
	20000, 30000, 40000, 50000,	75000, 100000, 200000, 300000, 400000, 500000
	};

	for (int size : workloadSizes) {
		std::vector<Point3D> syntheticVerts(size);
		for (int i = 0; i < size; i++) {
			float fi = (float)i;
			syntheticVerts[i] = Point3D(sinf(fi) * 10.0f, cosf(fi) * 10.0f, sinf(fi * 0.5f) * 10.0f);
		}
		std::string label = "synthetic (" + std::to_string(size / 1000) + "k)";
		benchmarkWorkload(label, syntheticVerts, mvp, rd.w, rd.h, iterations);
	}

	std::cout << "========================================================================================================\n";
	std::cout << "note: total speedup = cpu time / (h->d + kernel + d->h)\n";
	std::cout << "note: kernel speedup = cpu time / kernel time\n";
	std::cout << "========================================================================================================\n\n";
}