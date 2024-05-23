// Object.h
#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "typedefs.h"
#include "Mesh.h"
#include "ShaderProgram.h"
#include "printExtensions.h"
#include "Material.h"
#include <iostream>
#include <string>

class GameObject {
public:
	std::string name;
	Vector3 position;
	Vector3 scale;
	Quaternion rotation;
	ShaderProgram* shader;
    Mesh* mesh;
	Material* material;

	GameObject(ShaderProgram* program);
	GameObject();

	Matrix4 getModelingMatrix();
	
	void SetMesh(Mesh* mesh);
	void SetShader(ShaderProgram* shader);
	void SetPosition(Vector3 position);
	void SetScale(Vector3 scale);
	void SetRotation(Quaternion rotation);
	void SetMaterial(Material* material);

	Quaternion GetRotation();
	Vector3 GetPosition();
	Vector3 GetScale();

private:
	bool modelingMatrixDirty;
	Matrix4 modelingMatrix;
	void updateModelingMatrix();
};

#endif
