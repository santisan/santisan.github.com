#include "FirstPersonCamera.h"
#include <iostream>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float FirstPersonCamera::kDefaultRotationRate = glm::radians(0.01f);
const float FirstPersonCamera::kDefaultMovementRate = 1.0f;
const float FirstPersonCamera::kDefaultMouseSensitivity = 2.0f;

FirstPersonCamera::FirstPersonCamera()
	: Camera(), mMouseSensitivity(kDefaultMouseSensitivity), mRotationRate(kDefaultRotationRate), mMovementRate(kDefaultMovementRate),
	mInput(), mPrevMousePos()
{
}

FirstPersonCamera::FirstPersonCamera(const Input& input, float fieldOfView, float aspectRatio, float nearPlaneDistance, 
									 float farPlaneDistance)
    : Camera(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance), mMouseSensitivity(kDefaultMouseSensitivity), 
	mRotationRate(kDefaultRotationRate), mMovementRate(kDefaultMovementRate), mInput(input), mPrevMousePos()
{
}

FirstPersonCamera::~FirstPersonCamera()
{
}

float FirstPersonCamera::getMouseSensitivity() const
{
    return mMouseSensitivity;
}


float FirstPersonCamera::getRotationRate() const
{
    return mRotationRate;
}

float FirstPersonCamera::getMovementRate() const
{
    return mMovementRate;
}

void FirstPersonCamera::setInput(const Input& input)
{
	mInput = input;
}

void FirstPersonCamera::setMouseSensitivity(float value)
{
	mMouseSensitivity = value;
}

void FirstPersonCamera::setRotationRate(float value)
{
	mRotationRate = value;
}

void FirstPersonCamera::setMovementRate(float value)
{
	mMovementRate = value;
}

void FirstPersonCamera::initialize()
{
	Camera::initialize();
	mPrevMousePos = mInput.getMousePosition();
}

void FirstPersonCamera::update(double elapsedTime)
{
	glm::vec2 movementAmount = glm::vec2(0.0f);
	bool changedPosition = false;
    
	if (mInput.isKeyDown(GLFW_KEY_W)) {
        movementAmount.y = 1.0f;
		changedPosition = true;
    }

    if (mInput.isKeyDown(GLFW_KEY_S)) {
        movementAmount.y = -1.0f;
		changedPosition = true;
    }

    if (mInput.isKeyDown(GLFW_KEY_A)) {
        movementAmount.x = 1.0f;
		changedPosition = true;
    }

	if (mInput.isKeyDown(GLFW_KEY_D)) {
        movementAmount.x = -1.0f;
		changedPosition = true;
    }

	const float fElapsedTime = static_cast<float>(elapsedTime);
	bool changedOrientation = false;
	glm::vec2 mouseDelta(0.0f);

	//if (mInput.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
		const glm::vec2 mousePos = mInput.getMousePosition();
		mouseDelta = mousePos - mPrevMousePos;
		mPrevMousePos = mousePos;
		changedOrientation = (mouseDelta != glm::vec2(0.0f));
	//}

	if (changedOrientation) {
		const float pitch = mouseDelta.y * mMouseSensitivity * mRotationRate * fElapsedTime;
		const float yaw = -mouseDelta.x * mMouseSensitivity * mRotationRate * fElapsedTime;
		offsetOrientation(pitch, yaw);
	}

	if (changedPosition) {
		glm::vec2 movement = movementAmount * mMovementRate * fElapsedTime;
		glm::vec3 strafe = getRight() * movement.x;
		glm::vec3 forward = getForward() * movement.y;
		setPosition(getPosition() + strafe + forward);
	}

	Camera::update(elapsedTime);
}
