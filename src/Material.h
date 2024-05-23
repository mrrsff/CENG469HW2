#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "typedefs.h"

class Material
{
	public:
		Vector3 ambient;
		Vector3 diffuse;
		Vector3 specular;
		float shininess;

		Material();
		Material(Vector3 ambient, Vector3 diffuse, Vector3 specular, float shininess);
		~Material();
};

#endif