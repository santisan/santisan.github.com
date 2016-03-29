#pragma once
#include <GL/glew.h>
#include <unordered_map>
#include <string>

enum class TextureType
{
	DIFFUSE_MAP,
	NORMAL_MAP,
	SPECULAR_MAP
};

class Texture
{
public:
	Texture();
	~Texture();

	void bind(GLenum textureUnit) const;
	GLuint getId() const { return mId; }

	static const Texture* load(const std::string& fileName);
	static void unloadAll();
	static bool hasTexture(const char* textureName);
	static const Texture& get(const char* textureName);
	static void setBasePath(const std::string& basePath);
	static void setDefaultTexture(const Texture* pTexture);

	static const Texture* sDefaultTexture;

private:
	GLuint mId;
	std::string mName;

	static std::unordered_map<std::string, Texture> sTextureMap;
	static std::string sBasePath;

	static void unload(const Texture& texture);
};

