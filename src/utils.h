#ifndef UTILS_H
#define UTILS_H

#include "typedefs.h"
#include "GameObject.h"
#include "ShaderProgram.h"
#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <cassert>
#include <algorithm>

Mesh* ParseObjFile(const char* path, bool useTriangles, bool withNormals = false);
void CheckGLError(const char* file, int line);
Quaternion utilsLookAt(Vector3 position, Vector3 target, Vector3 up);
Quaternion utilsFromAxisAngle(Vector3 axis, double angle);
Vector3 utilsRotatePointAroundAxis(Vector3 point, Vector3 axis, double angle);

#endif