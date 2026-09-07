/*
 * calc.h - mathematical structures, matrix operations, and cpu/gpu benchmarking
 *
 * what this file does:
 * 1. defines 4x4 matrix struct with cuda host and device compatibility
 * 2. defines transform struct for model matrix generation
 * 3. defines camera struct for view matrix generation
 * 4. provides cpu vertex transformation functions with high-resolution timing
 * 5. provides comprehensive cpu vs cuda benchmark suite runner
 */

#pragma once
#include <vector>
#include "point.h"

// forward declarations
class Mesh;
struct rData;

// simple 4x4 matrix for 3d transformations with cuda host/device compatibility
struct Mat4 {
	float m[4][4];

	// default constructor initializes matrix elements to zero
	CUDA_HOST_DEV Mat4() {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m[i][j] = 0.0f;
			}
		}
	}

	// returns 4x4 identity matrix
	static Mat4 identity();

	// multiplies two 4x4 matrices
	static Mat4 multiply(const Mat4& a, const Mat4& b);

	// creates translation matrix from offsets
	static Mat4 makeTranslation(float tx, float ty, float tz);

	// creates scale matrix from factors
	static Mat4 makeScale(float sx, float sy, float sz);

	// creates x-axis rotation matrix from angle in degrees
	static Mat4 makeRotateX(float angleDeg);

	// creates y-axis rotation matrix from angle in degrees
	static Mat4 makeRotateY(float angleDeg);

	// creates z-axis rotation matrix from angle in degrees
	static Mat4 makeRotateZ(float angleDeg);

	// creates perspective projection matrix
	static Mat4 makePerspective(float fovDeg, float aspect, float nearZ, float farZ);
};

// object-specific transformation
struct Transform {
	Point3D position = { 0.0f, 0.0f, 0.0f };
	Point3D rotation = { 0.0f, 0.0f, 180.0f };
	Point3D scale = { 1.0f, 1.0f, 1.0f };

	// generates 4x4 model matrix
	Mat4 getModelMatrix() const;
};

// independent camera representation
struct Camera {
	Point3D target = { 0.0f, 0.0f, 0.0f };
	float yaw = 0.0f;
	float pitch = 20.0f;
	float distance = 25.0f;

	// generates 4x4 view matrix
	Mat4 getViewMatrix() const;
};

// transform single 3d point using 4x4 matrix and apply perspective division
bool transformPoint(const Mat4& mat, const Point3D& in, Point3D& out);

// map normalized device coordinates to screen pixel coordinates
Point2D screenMap(const Point3D& ndc, int w, int h);

// transform vector of 3d vertices on cpu and return execution time in milliseconds
float transformVerticesCPU(const std::vector<Point3D>& verts3D, std::vector<Point2D>& verts2D, const Mat4& mvp, int w, int h);

// run cpu vs cuda benchmark suite on multiple workload sizes and print results table
void runBenchmarkSuite(const Mesh& mesh, const rData& rd, int iterations = 50);