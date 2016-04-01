#include "RenderContext.h"
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 RenderContext::getCurrentWorldMatrix() const
{
	aiMatrix4x4 aiWorldMatrix = pAiNode->mTransformation;
	aiWorldMatrix.Transpose();
	return glm::make_mat4(aiWorldMatrix[0]);
}
