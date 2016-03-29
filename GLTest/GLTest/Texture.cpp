#include "Texture.h"
#include <FreeImage.h>
#include <assert.h>

std::unordered_map<std::string, Texture> Texture::sTextureMap;
std::string Texture::sBasePath;
const Texture* Texture::sDefaultTexture = nullptr;

GLenum getGLFormat(FREE_IMAGE_COLOR_TYPE type)
{
	assert(type == FIC_MINISBLACK || type == FIC_RGB || type == FIC_RGBALPHA);
	switch (type) {
		case FIC_MINISBLACK: return GL_RED;
		case FIC_RGB: return GL_RGB;
		case FIC_RGBALPHA: return GL_RGBA;
	};
	return GL_RGB;
}

Texture::Texture() :
	mId(0)
{
}

Texture::~Texture()
{
	mName.clear();
	mId = 0;
}

const Texture* Texture::load(const std::string& fileName)
{
	if (sTextureMap.find(fileName) != sTextureMap.end()) {
		unload(sTextureMap[fileName]);
	}

	const std::string path = sBasePath + fileName;

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path.c_str(), 0);
	if (fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(path.c_str());
	}
	if (fif == FIF_UNKNOWN) {
		return nullptr;
	}

	FIBITMAP* dib = nullptr;
	if (FreeImage_FIFSupportsReading(fif)) {
		dib = FreeImage_Load(fif, path.c_str());
	}
	if (!dib) {
		return nullptr;
	}

	BYTE* bits = FreeImage_GetBits(dib);
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	if (bits == 0 || width == 0 || height == 0) {
		return nullptr;
	}

	FREE_IMAGE_COLOR_TYPE colorType = FreeImage_GetColorType(dib);
	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
	GLenum glFormat = getGLFormat(colorType);

	Texture tex;
	tex.mName = fileName;
	glGenTextures(1, &tex.mId);
	assert(tex.mId);
	glBindTexture(GL_TEXTURE_2D, tex.mId);
	glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, GL_UNSIGNED_BYTE, bits);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	sTextureMap[fileName] = tex;

	FreeImage_Unload(dib);
	return &sTextureMap[fileName];
}

bool Texture::hasTexture(const char* textureName)
{
	return sTextureMap.find(textureName) != sTextureMap.end();
}

const Texture& Texture::get(const char* textureName)
{
	assert(hasTexture(textureName));
	return sTextureMap[textureName];
}

void Texture::unloadAll()
{
	for (const auto& it : sTextureMap) {
		unload(it.second);
	}
	sTextureMap.clear();
}

void Texture::setBasePath(const std::string& basePath)
{
	sBasePath = basePath;
}

void Texture::setDefaultTexture(const Texture* pTexture)
{
	sDefaultTexture = pTexture;
}

void Texture::unload(const Texture& texture)
{
	assert(texture.mId);
	glDeleteTextures(1, &texture.mId);
}

void Texture::bind(GLenum textureUnit) const
{
	assert(mId);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, mId);
}
