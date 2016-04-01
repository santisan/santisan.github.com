#pragma once
#include "RenderContext.h"

class Mesh;

class Renderer
{
public:
	RenderContext& getRenderContext() { return mRenderContext; }
	const RenderContext& getRenderContext() const { return mRenderContext; }

	void render(const Mesh& mesh);
	void renderDebug(const Mesh& mesh);

private:
	RenderContext mRenderContext;
};
