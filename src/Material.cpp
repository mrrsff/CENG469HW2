#include "Material.h"

Material::Material()
{
	ambient = Vector3(0.2f, 0.2f, 0.2f);
	diffuse = Vector3(0.8f, 0.8f, 0.8f);
	specular = Vector3(0.0f, 0.0f, 0.0f);
	shininess = 0.0f;
}

Material::Material(Vector3 ambient, Vector3 diffuse, Vector3 specular, float shininess)
{
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
	this->shininess = shininess;
}

Material::~Material()
{
}