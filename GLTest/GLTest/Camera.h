#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

class Camera
{
public:
    Camera(float aspectRatio);
    Camera(float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);

    virtual ~Camera();

    const glm::vec3& getPosition() const;
	float getPitch() const;
	float getYaw() const;
	glm::vec3 getRight() const;
	glm::vec3 getForward() const;
	glm::vec3 getUp() const;

    float getAspectRatio() const;
    float getFieldOfView() const;
    float getNearPlaneDistance() const;
    float getFarPlaneDistance() const;

    const glm::mat4& getViewMatrix();
    const glm::mat4& getProjectionMatrix();
    const glm::mat4& getViewProjectionMatrix();

    void setPosition(float x, float y, float z);
    void setPosition(const glm::vec3& position);
	void setOrientation(float pitch, float yaw);
	void offsetOrientation(float pitchOffset, float yawOffset);

    void reset();
    void initialize();

	virtual void update(double elapsedTime);

    static const float kDefaultFieldOfView;
    static const float kDefaultNearPlaneDistance;
    static const float kDefaultFarPlaneDistance;  

private:
	bool mIsViewDirty;
	bool mIsProjectionDirty;

    float mFieldOfView;
    float mAspectRatio;
    float mNearPlaneDistance;
    float mFarPlaneDistance;

    glm::vec3 mPosition;
	float mPitch;
	float mYaw;

	glm::mat4 mViewMatrix;
    glm::mat4 mProjectionMatrix;
	glm::mat4 mViewProjectionMatrix;

	void updateViewMatrix();
    void updateProjectionMatrix();
	void updateViewProjectionMatrix();

	Camera(const Camera& rhs);
    Camera& operator=(const Camera& rhs);
};
