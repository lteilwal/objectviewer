/*
 * renderdata.h - rendering context, camera, and profiling data
 *
 * what this file does:
 * 1. holds viewport dimensions and camera state
 * 2. holds model transformation properties
 * 3. holds compute mode selection (cpu vs gpu)
 * 4. holds continuous rotation state toggle
 * 5. holds runtime timing and profiling metrics for benchmarking
 */

#pragma once
#include "enum.h"
#include "calc.h"

// rendering context and profiling data container
struct rData {
	// window dimensions
	int w = 1280;
	int h = 720;

	// model transformation and camera view state
	Transform modelTransform;
	Camera camera;

	// active compute mode for vertex transformations
	computeMode mode = computeMode::CPU;

	// continuous model rotation flag
	bool continuousRotation = true;

	// runtime profiler metrics
	float frameTimeMs = 0.0f;
	float fps = 0.0f;
	float updateTimeMs = 0.0f;
	float transformTimeMs = 0.0f;
	float drawTimeMs = 0.0f;
	float presentTimeMs = 0.0f;

	// cuda specific breakdown timings in milliseconds
	float gpuH2DMs = 0.0f;
	float gpuKernelMs = 0.0f;
	float gpuD2HMs = 0.0f;
	float gpuTotalMs = 0.0f;

	// flags for toggling profiling output and triggering benchmark suite
	bool showProfiling = false;
	bool runBenchmarkRequested = false;
};