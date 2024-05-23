#ifndef CAMERA_H
#define CAMERA_H

#include "typedefs.h"
#include "printExtensions.h"
#include <memory>

enum CameraType {
	ORTHOGRAPHIC,
	PERSPECTIVE
};
class Camera {
	public: 
		void setPosition(Vector3 position);
		void setRotation(Quaternion rotation);
		void setFieldOfView(double fieldOfView);
		void setAspectRatio(double aspectRatio);
		void setNearPlane(double nearPlane);
		void setFarPlane(double farPlane);
		void setOrthoSize(double orthoSize);
		void setType(CameraType type);
		void setTarget(Vector3 target);
		Vector3 getPosition();
		Quaternion getRotation();
		Vector3 getUp();
		Vector3 getRight();
		Vector3 getForward();
		double getFieldOfView();
		double getAspectRatio();
		double getNearPlane();
		double getFarPlane();
		double getOrthoSize();
		CameraType getType();
		Matrix4* getViewMatrix();
		Matrix4* getProjectionMatrix();
		Camera();
		Camera(Vector3 position, Quaternion rotation, double fieldOfView, double aspectRatio, double nearPlane, double farPlane, double orthoSize, CameraType type);
		~Camera();
	private:
		bool viewMatrixDirty;
		bool projectionMatrixDirty;
		Vector3 position;
		Vector3 up;
		Vector3 right;
		Vector3 forward;
		Quaternion rotation;
		CameraType type;
		double fieldOfView;
		double aspectRatio;
		double nearPlane;
		double farPlane;
		double orthoSize;
		Matrix4 viewMatrix;
		Matrix4 projectionMatrix;
		void updateViewMatrix();	
		void updateProjectionMatrix();
};

#endif
