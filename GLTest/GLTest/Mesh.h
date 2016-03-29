#pragma once
#include <GL/glew.h>
#include <vector>
#include "GPUBuffers.h"

struct aiMesh;
struct aiScene;
class Material;

class Mesh
{
public:
	Mesh(const aiMesh& aiMesh, const Material& material);
	~Mesh();

	const Material& getMaterial() const { return mMaterial; }
	const VertexBuffer& getVertexBuffer() const { return mVertexBuffer; }
	const IndexBuffer& getIndexBuffer() const { return mIndexBuffer; }

	void createBuffers();
	void destroy();

private:
	const aiMesh& mAiMesh;
	VertexBuffer mVertexBuffer;
	IndexBuffer mIndexBuffer;
	const Material& mMaterial;

	void createIndexBuffer();
};

