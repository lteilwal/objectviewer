#include <stdio.h>
#include <SDL3/SDL.h>
#include <vector>
#include "renderdata.h"
#include "mesh.h"
#include "point.h"

__global__ void calcPoints(Point3D* dvert, rData rd, Point2D* dVert2D, int n) {
	const float M_PI = 3.14159265f;
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n) {
		Point3D temPt = dvert[i]; //meshRotateCalc
		float radian = rd.rotX * ((float)M_PI / 180.0f);//X rotation
		float oldY = temPt.y;
		float oldZ = temPt.z;
		float newY = oldZ * cos(radian) - oldY * sin(radian);
		float newZ = oldZ * sin(radian) + oldY * cos(radian);
		temPt.y = newY;
		temPt.z = newZ;

		radian = rd.rotY * ((float)M_PI / 180.0f);//Y rotation
		float oldX = temPt.x;
		oldZ = temPt.z;
		float newX = oldX * cos(radian) + oldZ * sin(radian);
		newZ = -oldX * sin(radian) + oldZ * cos(radian);
		temPt.x = newX;
		temPt.z = newZ;

		radian = rd.rotZ * ((float)M_PI / 180.0f);//Z rotation
		oldX = temPt.x;
		oldY = temPt.y;
		newX = oldX * cos(radian) - oldY * sin(radian);
		newY = oldX * sin(radian) + oldY * cos(radian);
		temPt.x = newX;
		temPt.y = newY;

		dVert2D[i].x = temPt.x / (temPt.z + rd.zoom); //Proj3Dto2D
		dVert2D[i].y = temPt.y / (temPt.z + rd.zoom); //Proj3Dto2D
		dVert2D[i].x = (dVert2D[i].x * rd.scale) + (rd.w / 2); //ScreenMap
		dVert2D[i].y = (dVert2D[i].y * rd.scale) + (rd.h / 2); //ScreenMap
	}
}

int kernelLaunchCalc(Mesh mesh, std::vector<Point2D>& verts2D, rData rd) {

	int vertCount = mesh.verts.size();
	if (vertCount == 0) {
		SDL_Log("Empty mesh");
		return 0;
	}

	int TPB = 256;
	int Blocks = (int)(ceil((float)vertCount / (float)TPB));
	int size = (int)(vertCount * (int)sizeof(Point3D));//Size of 3D vertex vector
	int size_out = (int)(vertCount * (int)sizeof(Point2D));//Size of 2D vertex vector

	Point3D* dVert3D;//vectors cannot be used in GPU
	Point2D* dVert2D;//vectors cannot be used in GPU
	verts2D.resize(vertCount);//since it starts empty

	cudaMalloc(&dVert3D, size);
	cudaMalloc(&dVert2D, size_out);

	cudaMemcpy(dVert3D, &mesh.verts[0], size, cudaMemcpyHostToDevice);

	calcPoints<<<Blocks, TPB>>>(dVert3D, rd, dVert2D, vertCount);

	cudaMemcpy(verts2D.data(), dVert2D, size_out, cudaMemcpyDeviceToHost);

	cudaFree(dVert3D);
	cudaFree(dVert2D);

	return 1;
}