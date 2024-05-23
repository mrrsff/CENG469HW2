// Mesh.h
#ifndef MESH_H
#define MESH_H

#include "typedefs.h"
#include "ShaderProgram.h"
#include "printExtensions.h"
#include <vector>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h> // The GLFW header

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct Vertex
{
	Vertex(GLfloat inX, GLfloat inY, GLfloat inZ) : x(inX), y(inY), z(inZ) { }
	GLfloat x, y, z;
};
struct Triangle
{
	Triangle(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	Triangle(std::vector<int> v, std::vector<int> t, std::vector<int> n) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
	}
	Triangle(std::vector<GLuint> v) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = v[0];
		tIndex[1] = v[1];
		tIndex[2] = v[2];
		nIndex[0] = v[0];
		nIndex[1] = v[1];
		nIndex[2] = v[2];
	}
	Triangle(std::vector<int> v, std::vector<int> t) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		nIndex[0] = v[0];
		nIndex[1] = v[1];
		nIndex[2] = v[2];
	}
	GLuint vIndex[3], tIndex[3], nIndex[3];
};
struct Quad
{
	Quad(int v[], int t[], int n[]) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		vIndex[3] = v[3];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		tIndex[3] = t[3];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
		nIndex[3] = n[3];
	}
	Quad(std::vector<int> v, std::vector<int> t, std::vector<int> n) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		vIndex[3] = v[3];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		tIndex[3] = t[3];
		nIndex[0] = n[0];
		nIndex[1] = n[1];
		nIndex[2] = n[2];
		nIndex[3] = n[3];
	}
	Quad(std::vector<GLuint> v) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		vIndex[3] = v[3];
		tIndex[0] = v[0];
		tIndex[1] = v[1];
		tIndex[2] = v[2];
		tIndex[3] = v[3];
		nIndex[0] = v[0];
		nIndex[1] = v[1];
		nIndex[2] = v[2];
		nIndex[3] = v[3];
	}
	Quad(std::vector<int> v, std::vector<int> t) {
		vIndex[0] = v[0];
		vIndex[1] = v[1];
		vIndex[2] = v[2];
		vIndex[3] = v[3];
		tIndex[0] = t[0];
		tIndex[1] = t[1];
		tIndex[2] = t[2];
		tIndex[3] = t[3];
		nIndex[0] = v[0];
		nIndex[1] = v[1];
		nIndex[2] = v[2];
		nIndex[3] = v[3];
	}
	GLuint vIndex[4], tIndex[4], nIndex[4];
};

class Mesh {
public:

	std::vector<Vector3>* vertices;
	std::vector<Vector2>* textures;
	std::vector<Vector3>* normals;
	std::vector<Triangle>* triangles;
	std::vector<Quad>* quads;
	bool quadMesh = false;

	Mesh();
	Mesh(std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& textures, std::vector<Quad>& quads);
    Mesh(std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& textures, std::vector<Triangle>& faces);

	Mesh* Clone();

	void UpdateMesh();
    void Draw();

	void DebugMeshInfo();

private:
	std::vector<int> glIndices;
	bool m_dirty = false;

    unsigned int VAO, VBO, EBO;

    void setupMesh();
};

#endif
