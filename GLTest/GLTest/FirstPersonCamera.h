#pragma once
#include "Camera.h"
#include <glm/vec2.hpp>
#include "Input.h"
    
class FirstPersonCamera : public Camera
{
public:	
    FirstPersonCamera();
    FirstPersonCamera(const Input& input, float fieldOfView, float aspectRatio, float nearPlaneDistance, float farPlaneDistance);

    ~FirstPersonCamera();

    float getMouseSensitivity() const;
    float getRotationRate() const;
    float getMovementRate() const;

	void setInput(const Input& input);
	void setMouseSensitivity(float value);
    void setRotationRate(float value);
    void setMovementRate(float value);
	
	void initialize() override;
    void update(double elapsedTime) override;

    static const float kDefaultMouseSensitivity;
    static const float kDefaultRotationRate;
    static const float kDefaultMovementRate;

private:
    float mMouseSensitivity;
    float mRotationRate;
    float mMovementRate;
	Input mInput;
	glm::vec2 mPrevMousePos;

    FirstPersonCamera(const FirstPersonCamera& rhs);
    FirstPersonCamera& operator=(const FirstPersonCamera& rhs);
};
