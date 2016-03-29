#pragma once

class Camera;
struct aiNode;

struct RenderContext
{
public:
	RenderContext() :
		pCamera(nullptr)
	{
	}

	Camera* pCamera;
	const aiNode* pAiNode;
};
