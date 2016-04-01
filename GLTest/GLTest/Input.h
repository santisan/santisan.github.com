#pragma once
#include <assert.h>
#include <GLFW/glfw3.h>

class Input
{
public:
	Input() : mpWindow(nullptr)
	{
	}

	Input(GLFWwindow* pWindow) :
		mpWindow(pWindow)
	{
	}

	bool isKeyDown(int keyCode) const
	{
		assert(mpWindow);
		int state = glfwGetKey(mpWindow, keyCode);
		return state == GLFW_PRESS;
	}

	bool isMouseButtonDown(int buttonCode) const
	{
		assert(mpWindow);
		int state = glfwGetMouseButton(mpWindow, buttonCode);
		return state == GLFW_PRESS;
	}

	glm::vec2 getMousePosition() const
	{
		assert(mpWindow);
		double x, y;
		glfwGetCursorPos(mpWindow, &x, &y);
		return glm::vec2(static_cast<float>(x), static_cast<float>(y));
	}

private:
	GLFWwindow* mpWindow;
};
