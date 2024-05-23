#include "printExtensions.h"

void printVector3(Vector3 vector)
{
	std::cout << "(" << std::fixed << std::setprecision(2) << vector.x << ", " << vector.y << ", " << vector.z << ")" << std::endl;
}

void printVector4(Vector4 vector)
{
	std::cout << "(" << std::fixed << std::setprecision(2) << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")" << std::endl;
}

void printMatrix3(Matrix3 matrix)
{
	std::cout << std::fixed << std::setprecision(2) << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << std::endl;
}

void printMatrix4(Matrix4 matrix)
{
	std::cout << std::fixed << std::setprecision(2) << matrix[0][0] << ", " << matrix[0][1] << ", " << matrix[0][2] << ", " << matrix[0][3] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[1][0] << ", " << matrix[1][1] << ", " << matrix[1][2] << ", " << matrix[1][3] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[2][0] << ", " << matrix[2][1] << ", " << matrix[2][2] << ", " << matrix[2][3] << std::endl;
	std::cout << std::fixed << std::setprecision(2) << matrix[3][0] << ", " << matrix[3][1] << ", " << matrix[3][2] << ", " << matrix[3][3] << std::endl;
}

void printQuaternion(Quaternion quaternion)
{
	std::cout << std::fixed << std::setprecision(2) << quaternion.x << ", " << quaternion.y << ", " << quaternion.z << ", " << quaternion.w << std::endl;
}