#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>
#include "typedefs.h"
#include "printExtensions.h"

class Light {
public:
	int id;
	Vector3 position; // position of the light source
	Vector3 color;
	float intensity;

	Light()
	{
		id = 0;
		position = Vector3(0.0f, 0.0f, 0.0f);
		color = Vector3(1.0f, 1.0f, 1.0f);
		intensity = 1.0f;
	}
};

#endif