/*
 * render.h - rendering calculations and stage drawing functions
 *
 * what this file does:
 * 1. coordinates vertex transformations via cpu or cuda based on compute mode
 * 2. renders transformed points and wireframe edges using sdl3 renderer
 * 3. renders minimal on-screen mode and performance overlay using sdl3 debug text
 */

#pragma once
#include <SDL3/SDL.h>
#include "point.h"
#include "mesh.h"
#include "renderdata.h"
#include <vector>

// transform all mesh vertices to 2d screen points using active compute mode
void transformMeshVertices(const Mesh& mesh, std::vector<Point2D>& screenPoints, rData& rd);

// render transformed points
void renderMeshPoints(SDL_Renderer* mren, const std::vector<Point2D>& screenPoints);

// render wireframe edges
void renderMeshEdges(SDL_Renderer* mren, const Mesh& mesh, const std::vector<Point2D>& screenPoints);

// render minimal on-screen mode and profiler overlay
void renderOverlay(SDL_Renderer* mren, const rData& rd);

// backward compatibility rendering functions
void meshPointsCalc(SDL_Renderer* mren, Mesh& mesh, rData& rd);
void meshEdgesCalc(SDL_Renderer* mren, Mesh& mesh, rData& rd);