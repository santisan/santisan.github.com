#include "Mesh.h"
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assert.h>
#include "Texture.h"
#include "Material.h"

Mesh::Mesh(const aiMesh& aiMesh, const Material& material) :
	mAiMesh(aiMesh),
	mMaterial(material)
{
}

Mesh::~Mesh()
{
}

void Mesh::createBuffers()
{
	mMaterial.createVertexBuffer(mAiMesh, mVertexBuffer);
	createIndexBuffer();
}

void Mesh::createIndexBuffer()
{
	assert(mAiMesh.HasFaces());
	assert(!mIndexBuffer.ElementBuffer);

	mIndexBuffer.IndexCount = mAiMesh.mNumFaces * 3;
	unsigned int* const indices = new unsigned int[mIndexBuffer.IndexCount];

	// Create index buffer
	unsigned int index = 0;
	for (unsigned int i=0; i < mAiMesh.mNumFaces; ++i) {
		assert(mAiMesh.mFaces[i].mNumIndices == 3);
		indices[index++] = mAiMesh.mFaces[i].mIndices[0];
		indices[index++] = mAiMesh.mFaces[i].mIndices[1];
		indices[index++] = mAiMesh.mFaces[i].mIndices[2];
	}

	glGenBuffers(1, &mIndexBuffer.ElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer.ElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer.IndexCount * sizeof(indices[0]), indices, GL_STATIC_DRAW);

	delete[] indices;
}

void Mesh::destroy()
{
	glDeleteBuffers(static_cast<GLsizei>(mVertexBuffer.VBOs.size()), &mVertexBuffer.VBOs[0]);
	glDeleteBuffers(1, &mIndexBuffer.ElementBuffer);
	glDeleteVertexArrays(1, &mVertexBuffer.VAO);

	mVertexBuffer.clear();
	mIndexBuffer.clear();
}
