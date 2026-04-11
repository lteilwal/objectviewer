#pragma once
#include <SDL3/SDL.h>
#include "point.h"
#include "mesh.h"
#include "renderdata.h"

void meshPointsCalc(SDL_Renderer* mren, Mesh& mesh, rData rd);
void meshEdgesCalc(SDL_Renderer* mren, Mesh& mesh, rData rd);
Point3D meshRotateCalc(const Point3D& pt, rData rd);

void meshPointsRender(SDL_Renderer* mren, const std::vector<Point2D>& screenPoint2D);
void meshEdgesRender(SDL_Renderer* mren, Point2D screenPoint2D1, Point2D screenPoint2D2);