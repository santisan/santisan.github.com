#include "Material.h"
#include <assimp/mesh.h>
#include <assimp/material.h>
#include <assimp/scene.h>
#include <assert.h>
#include <glm/gtc/type_ptr.hpp>
#include "GPUBuffers.h"
#include "GPUProgram.h"
#include "Texture.h"
#include "RenderContext.h"
#include "Camera.h"

Material::Material(const aiMaterial& aiMaterial, const GPUProgram& program) :
	mAiMaterial(aiMaterial),
	mGPUProgram(program)
{
}

Material::~Material()
{
	mTextures.clear();
}

void Material::init()
{
	loadTexture(TextureType::DIFFUSE_MAP);
	loadTexture(TextureType::NORMAL_MAP);
	loadTexture(TextureType::SPECULAR_MAP);
}

aiTextureType getTextureType(TextureType type)
{
	switch (type) {
		case TextureType::DIFFUSE_MAP:
			return aiTextureType_DIFFUSE;
		case TextureType::NORMAL_MAP:
			return aiTextureType_HEIGHT; // For OBJs the type is HEIGHT not NORMAL
		case TextureType::SPECULAR_MAP:
			return aiTextureType_SPECULAR;
	}
	return aiTextureType_DIFFUSE;
}

void Material::loadTexture(TextureType textureType)
{
	const aiTextureType aiType = getTextureType(textureType);
	aiString path;
	aiReturn texFound = mAiMaterial.GetTexture(aiType, 0, &path);
	bool useDefaultTexture = false;
	if (texFound == AI_SUCCESS) {
		const Texture* const pTexture = Texture::load(path.C_Str());
		if (pTexture) {
			addTexture(textureType, pTexture);
		}
		else {
			fprintf(stderr, "Error loading texture: %s\n", path.C_Str());
			useDefaultTexture = true;
		}
	}
	else {
		useDefaultTexture = true;
	}

	if (useDefaultTexture) {
		addTexture(textureType, Texture::sDefaultTexture);
	}
}

bool Material::hasTexture(TextureType type) const
{
	return mTextures.find(type) != mTextures.end();
}

const Texture& Material::getTexture(TextureType type) const
{
	const auto& it = mTextures.find(type);
	assert(it != mTextures.end());
	return *it->second;
}

void Material::addTexture(TextureType type, const Texture* pTexture)
{
	mTextures.insert(std::make_pair(type, pTexture));
}

void Material::apply(const RenderContext& renderContext) const
{
	assert(renderContext.pCamera);
	mGPUProgram.use();

	auto& it = mTextures.find(TextureType::DIFFUSE_MAP);
	if (it != mTextures.end()) {
		it->second->bind(GL_TEXTURE0);
		mGPUProgram.setUniform("DiffuseMap", 0);
	}

	it = mTextures.find(TextureType::NORMAL_MAP);
	if (it != mTextures.end()) {
		it->second->bind(GL_TEXTURE1);
		mGPUProgram.setUniform("NormalMap", 1);
	}	

	it = mTextures.find(TextureType::SPECULAR_MAP);
	if (it != mTextures.end()) {
		it->second->bind(GL_TEXTURE2);
		mGPUProgram.setUniform("SpecularMap", 2);
	}	

	aiMatrix4x4 aiWorldMatrix = renderContext.pAiNode->mTransformation;
	aiWorldMatrix.Transpose();
	glm::mat4 worldMatrix = glm::make_mat4(aiWorldMatrix[0]);
	glm::mat4 wvpMatrix = renderContext.pCamera->getViewProjectionMatrix() * worldMatrix;

	mGPUProgram.setUniform("WorldMatrix", worldMatrix);
	mGPUProgram.setUniform("WVPMatrix", wvpMatrix);
	mGPUProgram.setUniform("CameraPosition", renderContext.pCamera->getPosition());
}

void Material::createVertexBuffer(const aiMesh& aiMesh, VertexBuffer& vertexBuffer) const
{
	assert(aiMesh.HasPositions());
	assert(aiMesh.HasNormals());
	assert(aiMesh.HasTangentsAndBitangents());
	assert(aiMesh.HasTextureCoords(0));

	vertexBuffer.VertexCount = aiMesh.mNumVertices;
	const unsigned int vertexDataSize = vertexBuffer.VertexCount * 3;
	const unsigned int uvDataSize = vertexBuffer.VertexCount * aiMesh.mNumUVComponents[0];
	float* const positionData = new float[vertexDataSize];
	float* const normalData = new float[vertexDataSize];
	float* const tangentData = new float[vertexDataSize];
	float* const uvData = new float[uvDataSize];

	unsigned int index = 0, uvIndex = 0;
	for (unsigned int i=0; i < aiMesh.mNumVertices; ++i, index += 3, uvIndex += aiMesh.mNumUVComponents[0]) {
		positionData[index] = aiMesh.mVertices[i].x;
		positionData[index + 1] = aiMesh.mVertices[i].y;
		positionData[index + 2] = aiMesh.mVertices[i].z;

		normalData[index] = aiMesh.mNormals[i].x;
		normalData[index + 1] = aiMesh.mNormals[i].y;
		normalData[index + 2] = aiMesh.mNormals[i].z;

		tangentData[index] = aiMesh.mTangents[i].x;
		tangentData[index + 1] = aiMesh.mTangents[i].y;
		tangentData[index + 2] = aiMesh.mTangents[i].z;

		uvData[uvIndex] = aiMesh.mTextureCoords[0][i].x;
		if (aiMesh.mNumUVComponents[0] > 1) {
			uvData[uvIndex + 1] = aiMesh.mTextureCoords[0][i].y;
			if (aiMesh.mNumUVComponents[0] > 2) {
				uvData[uvIndex + 2] = aiMesh.mTextureCoords[0][i].z;
			}
		}
	}

	vertexBuffer.VBOs.resize(4);
	glGenBuffers(4, &vertexBuffer.VBOs[0]);
	GLuint positionVBO = vertexBuffer.VBOs[0];
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize * sizeof(positionData[0]), positionData, GL_STATIC_DRAW);
	GLuint normalVBO = vertexBuffer.VBOs[1];
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize * sizeof(normalData[0]), normalData, GL_STATIC_DRAW);
	GLuint tangentVBO = vertexBuffer.VBOs[2];
	glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexDataSize * sizeof(tangentData[0]), tangentData, GL_STATIC_DRAW);
	GLuint texCoordVBO = vertexBuffer.VBOs[3];
	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glBufferData(GL_ARRAY_BUFFER, uvDataSize * sizeof(uvData[0]), uvData, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vertexBuffer.VAO);
	glBindVertexArray(vertexBuffer.VAO);
	for (unsigned int i=0; i < vertexBuffer.VBOs.size(); ++i) {
		glEnableVertexAttribArray(i);
	}
	glBindBuffer(GL_ARRAY_BUFFER, positionVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);	
	glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
	glVertexAttribPointer(1, aiMesh.mNumUVComponents[0], GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	delete[] positionData;
	delete[] normalData;
	delete[] tangentData;
	delete[] uvData;
}
