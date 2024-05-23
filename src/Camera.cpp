#include "Camera.h"

void Camera::setPosition(Vector3 position) {
	this->position.x = position.x;
	this->position.y = position.y;
	this->position.z = position.z;
	viewMatrixDirty = true;
}
void Camera::setRotation(Quaternion rotation) {
	this->rotation = rotation;
	viewMatrixDirty = true;
	// Calculate up, right, and forward vectors
	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
	up = glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0));
	right = glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0));
	forward = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));
}
void Camera::setFieldOfView(double fieldOfView) {
	this->fieldOfView = fieldOfView;
	projectionMatrixDirty = true;
}
void Camera::setAspectRatio(double aspectRatio) {
	this->aspectRatio = aspectRatio;
	projectionMatrixDirty = true;
}
void Camera::setNearPlane(double nearPlane) {
	this->nearPlane = nearPlane;
	projectionMatrixDirty = true;
}
void Camera::setFarPlane(double farPlane) {
	this->farPlane = farPlane;
	projectionMatrixDirty = true;
}
void Camera::setOrthoSize(double orthoSize) {
	this->orthoSize = orthoSize;
	projectionMatrixDirty = true;
}
void Camera::setType(CameraType type) {
	this->type = type;
	projectionMatrixDirty = true;
}
void Camera::setTarget(Vector3 target) {
	forward = glm::normalize(target - position);
	right = glm::normalize(glm::cross(forward, Vector3(0, 1, 0)));
	up = glm::normalize(glm::cross(forward, right));
	rotation = glm::quatLookAt(forward, up);
	viewMatrixDirty = true;
}
Vector3 Camera::getPosition() {
	return position;
}
Quaternion Camera::getRotation() {
	return rotation;
}
Vector3 Camera::getUp() {
	return up;
}
Vector3 Camera::getRight() {
	return right;
}
Vector3 Camera::getForward() {
	return forward;
}
double Camera::getFieldOfView() {
	return fieldOfView;
}
double Camera::getAspectRatio() {
	return aspectRatio;
}
double Camera::getNearPlane() {
	return nearPlane;
}
double Camera::getFarPlane() {
	return farPlane;
}
double Camera::getOrthoSize() {
	return orthoSize;
}
CameraType Camera::getType() {
	return type;
}
Matrix4* Camera::getViewMatrix() {
	if (viewMatrixDirty) {
		updateViewMatrix();
	}
	return &viewMatrix;
}
Matrix4* Camera::getProjectionMatrix() {
	if (projectionMatrixDirty) {
		updateProjectionMatrix();
	}
	return &projectionMatrix;
}
Camera::Camera() 
	: position(Vector3(0, 0, 0)), rotation(Quaternion(0, 0, 0, 1)), fieldOfView(60), aspectRatio(16.0/9.0), nearPlane(0.1), farPlane(1000), orthoSize(5), type(PERSPECTIVE)
{
	updateViewMatrix();
	updateProjectionMatrix();
}
Camera::Camera(Vector3 position, Quaternion rotation, double fieldOfView, double aspectRatio, double nearPlane, double farPlane, double orthoSize, CameraType type) 
	: position(position), rotation(rotation), fieldOfView(fieldOfView), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane), orthoSize(orthoSize), type(type) 
{
	updateViewMatrix();
	updateProjectionMatrix();
}
Camera::~Camera() {}
void Camera::updateViewMatrix() {
	viewMatrixDirty = false;

	// Calculate up, right, and forward vectors
	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);
	up = glm::vec3(rotationMatrix * glm::vec4(0, 1, 0, 0));
	right = glm::vec3(rotationMatrix * glm::vec4(1, 0, 0, 0));
	forward = glm::vec3(rotationMatrix * glm::vec4(0, 0, -1, 0));
	
	// Calculate view matrix
	viewMatrix = glm::lookAt(glm::vec3(position.x, position.y, position.z), glm::vec3(position.x, position.y, position.z) + forward, up);
}
void Camera::updateProjectionMatrix() {
	projectionMatrixDirty = false;
	if (type == PERSPECTIVE) {
		projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, nearPlane, farPlane);
	}
	else {
		projectionMatrix = glm::ortho(-orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, nearPlane, farPlane);
	}
}
// #endif