#pragma once
#include <unordered_map>

struct aiMesh;
struct aiMaterial;
struct VertexBuffer;
struct RenderContext;
enum class TextureType;
class Texture;
class GPUProgram;

class Material
{
public:
	Material(const aiMaterial& aiMaterial, const GPUProgram& program);
	virtual ~Material();

	const GPUProgram& getGPUProgram() const { return mGPUProgram; }
	bool hasTexture(TextureType type) const;
	const Texture& getTexture(TextureType type) const;
	void addTexture(TextureType type, const Texture* pTexture);

	virtual void init();
	virtual void apply(const RenderContext& renderContext) const;
	virtual void createVertexBuffer(const aiMesh& aiMesh, VertexBuffer& vertexBuffer) const;

private:
	const aiMaterial& mAiMaterial;
	std::unordered_map<TextureType, const Texture*> mTextures;
	const GPUProgram& mGPUProgram;

	void loadTexture(TextureType textureType);
};

