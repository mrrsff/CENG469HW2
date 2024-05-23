// File: Mesh.cpp
#include "Mesh.h"
Mesh::Mesh() {
	this->vertices = new std::vector<Vector3>();
	this->normals = new std::vector<Vector3>();
	this->textures = new std::vector<Vector2>();
	this->triangles = new std::vector<Triangle>();
	this->quads = new std::vector<Quad>();
	quadMesh = false;
}
Mesh::Mesh(std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& textures, std::vector<Quad>& quads)
{
	this->vertices = &vertices;
	this->normals = &normals;
	this->textures = &textures;
	this->quads = &quads;
	this->triangles = new std::vector<Triangle>();
	quadMesh = true;

	this->setupMesh();
}
Mesh::Mesh(std::vector<Vector3>& vertices, std::vector<Vector3>& normals, std::vector<Vector2>& textures, std::vector<Triangle>& triangles) {
	this->vertices = &vertices;
	this->normals = &normals;
	this->textures = &textures;
	this->triangles = &triangles;
	this->quads = new std::vector<Quad>();
	quadMesh = false;

	this->setupMesh();
}
Mesh* Mesh::Clone() {
	Mesh *newMesh = new Mesh();
	newMesh->vertices = new std::vector<Vector3>(*vertices);
	newMesh->normals = new std::vector<Vector3>(*normals);
	newMesh->textures = new std::vector<Vector2>(*textures);
	newMesh->triangles = new std::vector<Triangle>(*triangles);
	newMesh->quads = new std::vector<Quad>(*quads);
	newMesh->quadMesh = quadMesh;
    return newMesh;
}

void Mesh::UpdateMesh() {
	this->setupMesh();
}
void Mesh::Draw() {
	if (m_dirty) {
		setupMesh();
	}
	glBindVertexArray(VAO);
	assert(glGetError() == GL_NO_ERROR);

	glDrawElements(GL_TRIANGLES, glIndices.size(), GL_UNSIGNED_INT, 0); // 3 indices starting at 0 -> 1 triangle

	glBindVertexArray(0);

	assert(glGetError() == GL_NO_ERROR);
}
void Mesh::setupMesh() {
	m_dirty = false;

	// Upload data to directly to GPU, without any processing
	// Create VAO, VBO, EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind VAO
	glBindVertexArray(VAO);
	
	// Cache sizes
	int verticesSize = vertices->size() * sizeof(Vector3);
	int normalsSize = normals->size() * sizeof(Vector3);
	int texturesSize = textures->size() * sizeof(Vector2);
	int indicesSize = triangles->size() * 3 * sizeof(int); // Always render triangles

	// Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, verticesSize + normalsSize + texturesSize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, vertices->data());
	glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, normals->data());
	glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, texturesSize, textures->data());

	// Bind EBO
	std::vector<int> newIndices;
	newIndices.reserve(triangles->size() * 3);
	for (Triangle t : *triangles) {
		for (int i = 0; i < 3; i++) {
			newIndices.push_back(t.vIndex[i]);
		}
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, newIndices.data(), GL_DYNAMIC_DRAW);
	
	// Set vertex attributes
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
	// Normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(verticesSize));
	glEnableVertexAttribArray(1);
	// Texture attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(verticesSize + normalsSize));
	glEnableVertexAttribArray(2);

	// Unbind VAO
	glBindVertexArray(0);
	glIndices = std::vector<int>(newIndices.size());
	for (int i = 0; i < newIndices.size(); ++i)
	{
		glIndices[i] = newIndices[i];
	}

	// Unbind VBO and EBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	assert(glGetError() == GL_NO_ERROR);
	
	// OLD CODE
	// Create new vertices and normals
	// std::vector<Vector3> newVertices;
	// std::vector<Vector3> newNormals;
	// std::vector<Vector2> newTextures;
	// std::vector<int> newIndices;

	// newVertices.reserve(triangles->size() * 3);
	// newNormals.reserve(triangles->size() * 3);
	// newTextures.reserve(triangles->size() * 3);
	// newIndices.reserve(triangles->size() * 3);

	// if (quadMesh) {
	// 	for (Quad q : *quads) {
	// 		for (int i = 0; i < 4; i++) {
	// 			newVertices.push_back(vertices->at(q.vIndex[i]));
	// 			newNormals.push_back(normals->at(q.nIndex[i]));
	// 			newIndices.push_back(newIndices.size());
	// 		}
	// 	}
	// }
	// else {
	// 	for (Triangle t : *triangles) {
	// 		for (int i = 0; i < 3; i++) {
	// 			newVertices.push_back(vertices->at(t.vIndex[i]));
	// 			newNormals.push_back(normals->at(t.nIndex[i]));
	// 			newIndices.push_back(newIndices.size());
	// 		}
	// 	}
	// }

	// // Create VAO, VBO, EBO
	// glGenVertexArrays(1, &VAO);	
	// glGenBuffers(1, &VBO);
	// glGenBuffers(1, &EBO);

	// // Bind VAO
	// glBindVertexArray(VAO);

	// // Cache sizes
	// int verticesSize = newVertices.size() * sizeof(Vector3);
	// int normalsSize = newNormals.size() * sizeof(Vector3);
	// int texturesSize = newTextures.size() * sizeof(Vector2);
	// int indicesSize = newIndices.size() * sizeof(int);

	// // Bind VBO
	// glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferData(GL_ARRAY_BUFFER, verticesSize + normalsSize + texturesSize, NULL, GL_STATIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, verticesSize, newVertices.data());
	// glBufferSubData(GL_ARRAY_BUFFER, verticesSize, normalsSize, newNormals.data());
	// glBufferSubData(GL_ARRAY_BUFFER, verticesSize + normalsSize, texturesSize, newTextures.data());

	// // Bind EBO
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, newIndices.data(), GL_DYNAMIC_DRAW);

	// // Set vertex attributes
	// // Position attribute
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	// glEnableVertexAttribArray(0);
	// // Normal attribute
	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(verticesSize));
	// glEnableVertexAttribArray(1);
	// // Texture attribute
	// glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)(verticesSize + normalsSize));
	// glEnableVertexAttribArray(2);

	// // Unbind VAO
	// glBindVertexArray(0);
	// glIndices = std::vector<int>(newIndices.size());
	// for (int i = 0; i < newIndices.size(); ++i)
	// {
	// 	glIndices[i] = newIndices[i];
	// }

	// // Unbind VBO and EBO
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// assert(glGetError() == GL_NO_ERROR);
}

void Mesh::DebugMeshInfo() {
	std::cout << "Mesh(DEBUG): " << std::endl;   
	// Print all vertices, all normals, all textures, all triangles
	// Triangles
	// std::cout << "DEBUG Triangles: " << std::endl;
	// for (Triangle t : *triangles) {
	// 	std::cout << "Triangle: " << t.vIndex[0] << " " << t.vIndex[1] << " " << t.vIndex[2] << std::endl;
	// }
}
