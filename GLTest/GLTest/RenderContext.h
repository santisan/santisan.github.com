#pragma once
#include <glm/mat4x4.hpp>

class Camera;
struct aiNode;

struct RenderContext
{
public:
	RenderContext() :
		pCamera(nullptr)
	{
	}

	glm::mat4 getCurrentWorldMatrix() const;

	Camera* pCamera;
	const aiNode* pAiNode;
	float Time;
};
