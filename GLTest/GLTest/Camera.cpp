#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

const float Camera::kDefaultFieldOfView = glm::quarter_pi<const float>();
const float Camera::kDefaultNearPlaneDistance = 0.01f;
const float Camera::kDefaultFarPlaneDistance = 10000.0f;

Camera::Camera() :
	mIsViewDirty(true), mIsProjectionDirty(true), mFieldOfView(kDefaultFieldOfView), mAspectRatio(1.0f), 
	mNearPlaneDistance(kDefaultNearPlaneDistance), mFarPlaneDistance(kDefaultFarPlaneDistance), mPosition(0.0f), mPitch(0.0f), mYaw(0.0f),
	mViewMatrix(), mProjectionMatrix()
{
}

Camera::Camera(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance) : 
    mIsViewDirty(true), mIsProjectionDirty(true), mFieldOfView(fieldOfView), mAspectRatio(aspectRatio), mNearPlaneDistance(nearPlaneDistance), 
	mFarPlaneDistance(farPlaneDistance), mPosition(0.0f), mPitch(0.0f), mYaw(0.0f), mViewMatrix(), mProjectionMatrix()
{
}

Camera::~Camera()
{
}
    
const glm::vec3& Camera::getPosition() const
{
    return mPosition;
}

float Camera::getPitch() const
{
	return mPitch;
}

float Camera::getYaw() const
{
	return mYaw;
}

glm::vec3 Camera::getRight() const
{
	glm::vec3 right(1.0f, 0.0f, 0.0);
	const glm::vec3 eulerAngles(mPitch, mYaw, 0.0f);
	right = glm::quat(eulerAngles) * right; 
	return right;
}

glm::vec3 Camera::getForward() const
{
	glm::vec3 forward(0.0f, 0.0f, 1.0);
	const glm::vec3 eulerAngles(mPitch, mYaw, 0.0f);
	forward = glm::quat(eulerAngles) * forward;
	return forward;
}

glm::vec3 Camera::getUp() const
{
	glm::vec3 up(0.0f, 1.0f, 0.0);
	const glm::vec3 eulerAngles(mPitch, mYaw, 0.0f);
	up = glm::quat(eulerAngles) * up;
	return up;
}

float Camera::getAspectRatio() const
{
    return mAspectRatio;
}

float Camera::getFieldOfView() const
{
    return mFieldOfView;
}

float Camera::getNearPlaneDistance() const
{
    return mNearPlaneDistance;
}

float Camera::getFarPlaneDistance() const
{
    return mFarPlaneDistance;
}

const glm::mat4& Camera::getViewMatrix()
{
	if (mIsViewDirty) {
		updateViewMatrix();
	}
    return mViewMatrix;
}

const glm::mat4& Camera::getProjectionMatrix()
{
	if (mIsProjectionDirty) {
		updateProjectionMatrix();
	}
    return mProjectionMatrix;
}

const glm::mat4& Camera::getViewProjectionMatrix()
{
	if (mIsViewDirty || mIsProjectionDirty) {
		update(0.0f);
	}
	return mViewProjectionMatrix;
}

void Camera::setFieldOfView(float value)
{
	mFieldOfView = value;
}

void Camera::setAspectRatio(float value)
{
	mAspectRatio = value;
}

void Camera::setNearPlaneDistance(float value)
{
	mNearPlaneDistance = value;
}

void Camera::setFarPlaneDistance(float value)
{
	mFarPlaneDistance = value;
}

void Camera::setPosition(float x, float y, float z)
{
	setPosition(glm::vec3(x, y, z));
}

void Camera::setPosition(const glm::vec3& position)
{
	mPosition = position;
	mIsViewDirty = true;
}

void Camera::setOrientation(float pitch, float yaw)
{
	mPitch = pitch;
	mYaw = yaw;
	mIsViewDirty = true;
}

void Camera::offsetOrientation(float pitchOffset, float yawOffset)
{
	mPitch += pitchOffset;
	mYaw += yawOffset;
	mIsViewDirty = true;
}

void Camera::reset()
{
    mPosition = glm::vec3(0.0f);
	mPitch = 0.0f;
	mYaw = 0.0f;
	mIsViewDirty = true;
        
    updateViewMatrix();
}

void Camera::initialize()
{
	updateProjectionMatrix();
    reset();
}

void Camera::update(double elapsedTime)
{
	bool shouldUpdateViewProjection = mIsViewDirty || mIsProjectionDirty;

    updateViewMatrix();
	updateProjectionMatrix();
	
	if (shouldUpdateViewProjection) {
		updateViewProjectionMatrix();
	}
}

void Camera::updateViewMatrix()
{
	if (mIsViewDirty) {
		mViewMatrix = glm::lookAt(mPosition, mPosition + getForward(), getUp());
		mIsViewDirty = false;
	}
}

void Camera::updateProjectionMatrix()
{
	if (mIsProjectionDirty) {
		mProjectionMatrix = glm::perspective(mFieldOfView, mAspectRatio, mNearPlaneDistance, mFarPlaneDistance);
		mIsProjectionDirty = false;
	}
}

void Camera::updateViewProjectionMatrix()
{
	mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
}
