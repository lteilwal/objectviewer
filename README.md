# MeshViewer

A lightweight real-time 3D mesh viewer built with **C++17, SDL3, and CUDA**.

Loads Wavefront `.obj` models and renders them using a custom 3D transformation and projection pipeline. Supports both CPU and CUDA vertex processing, with benchmarking to compare performance across different workloads.

## Features

- Wavefront `.obj` mesh loading
- Custom 4x4 matrix transformations
- Perspective projection
- Orbit camera and zoom
- CPU and CUDA compute modes
- Back-face culling and depth-based visibility
- Wireframe rendering
- Continuous model rotation
- On-screen performance statistics
- CPU vs CUDA benchmarking up to 500K vertices

## Controls

| Input | Action |
|---|---|
| Left mouse + drag | Orbit camera |
| Mouse wheel | Zoom |
| `R` | Toggle model rotation |
| `M` | Toggle CPU / CUDA |
| `P` | Toggle performance overlay |
| `B` | Run CPU vs CUDA benchmark |

## Benchmark

The benchmark compares end-to-end CPU and CUDA execution time, including GPU host-to-device transfers, kernel execution, and device-to-host transfers.

On the test system, CUDA becomes competitive at around **50K vertices** and reaches approximately **2.56× end-to-end speedup at 500K vertices**.

## Build

### Requirements

- C++17 compiler
- CMake
- SDL3
- NVIDIA CUDA Toolkit
- NVIDIA GPU for CUDA mode

### Build

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release