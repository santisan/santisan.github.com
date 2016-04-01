#include <string>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "GPUProgram.h"
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "FirstPersonCamera.h"
#include "Renderer.h"
#include "Input.h"

using glm::mat4;
using glm::vec3;
using glm::vec2;

//#define DEBUG_DRAW

class GLTest
{
	GLFWwindow* mpWindow;
	GPUProgram mGPUProgram;
	FirstPersonCamera mCamera;
	std::unordered_map<const aiMesh*, Mesh> mMeshMap;
	std::unordered_map<const aiMaterial*, Material> mMaterialMap;
	Renderer mRenderer;

	int printOglError(char *file, int line)
	{
		GLenum glErr;
		int    retCode = 0;

		glErr = glGetError();
		if (glErr != GL_NO_ERROR)
		{
			printf("glError in file %s @ line %d: %s\n", file, line, glewGetErrorString(glErr));
			retCode = 1;
		}
		return retCode;
	}

	#define printOpenGLError() printOglError(__FILE__, __LINE__)

	void processSceneNode(const aiScene* const pScene, const aiNode* const pNode)
	{
		assert(pNode);
		for (unsigned int m=0; m < pNode->mNumMeshes; ++m) {
			const aiMesh* const pAiMesh = pScene->mMeshes[pNode->mMeshes[m]];
			assert(pAiMesh);

			if (mMeshMap.find(pAiMesh) == mMeshMap.end()) {
				const aiMaterial* const pAiMaterial = pScene->mMaterials[pAiMesh->mMaterialIndex];
				assert(pAiMaterial);

				Material* pMaterial = nullptr;
				auto& matIt = mMaterialMap.find(pAiMaterial);
				if (matIt != mMaterialMap.end()) {
					pMaterial = &matIt->second;
				}
				else {
					mMaterialMap.insert(std::make_pair(pAiMaterial, Material(*pAiMaterial, mGPUProgram)));
					pMaterial = &mMaterialMap.at(pAiMaterial);
					pMaterial->init();
				}

				assert(pMaterial);
				mMeshMap.insert(std::make_pair(pAiMesh, Mesh(*pAiMesh, *pMaterial)));
				Mesh* pMesh = &mMeshMap.at(pAiMesh);
				pMesh->createBuffers();
			}
		}

		for (unsigned int n=0; n < pNode->mNumChildren; ++n) {
			processSceneNode(pScene, pNode->mChildren[n]);
		}
	}

	void renderSceneNode(const aiScene* const pScene, const aiNode* const pNode)
	{
		assert(pNode);
		mRenderer.getRenderContext().pAiNode = pNode;

		for (unsigned int m=0; m < pNode->mNumMeshes; ++m) {
			const aiMesh* const pAiMesh = pScene->mMeshes[pNode->mMeshes[m]];
			assert(pAiMesh);
			mRenderer.render(mMeshMap.at(pAiMesh));
		}

		for (unsigned int n=0; n < pNode->mNumChildren; ++n) {
			renderSceneNode(pScene, pNode->mChildren[n]);
		}
	}

	void renderSceneNodeDebug(const aiScene* const pScene, const aiNode* const pNode)
	{
#if defined(DEBUG_DRAW)
		assert(pNode);
		mRenderer.getRenderContext().pAiNode = pNode;

		for (unsigned int m=0; m < pNode->mNumMeshes; ++m) {
			const aiMesh* const pAiMesh = pScene->mMeshes[pNode->mMeshes[m]];
			assert(pAiMesh);
			mRenderer.renderDebug(mMeshMap.at(pAiMesh));
		}

		for (unsigned int n=0; n < pNode->mNumChildren; ++n) {
			renderSceneNodeDebug(pScene, pNode->mChildren[n]);
		}
#endif
	}

	void destroyMeshes()
	{
		for (auto& meshMapIt : mMeshMap) {
			meshMapIt.second.destroy();
		}
	}

	static void errorCallback(int error, const char* description)
	{
		fprintf(stderr, "GLFW error: %s\n", description);
	}

public:
	static GLTest sTheApp;

	GLTest() :
		mpWindow(nullptr)
	{
	}

	int run()
	{
		if (!glfwInit()) {
			return -1;
		}

		glfwSetErrorCallback(GLTest::errorCallback);

		mpWindow = glfwCreateWindow(1024, 768, "Hello World", NULL, NULL);
		if (!mpWindow) {
			glfwTerminate();
			return -1;
		}

		glfwMakeContextCurrent(mpWindow);

		GLenum err = glewInit();
		if (err != GLEW_OK) {
			fprintf(stderr, "Error initializing GLEW: %s\n", glewGetErrorString(err));
		}

		const GLubyte *renderer = glGetString( GL_RENDERER );
		const GLubyte *vendor = glGetString( GL_VENDOR );
		const GLubyte *version = glGetString( GL_VERSION );
		const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);
		printf("GL Vendor : %s\n", vendor);
		printf("GL Renderer : %s\n", renderer);
		printf("GL Version (string) : %s\n", version);
		printf("GL Version (integer) : %d.%d\n", major, minor);
		printf("GLSL Version : %s\n", glslVersion);

		int width, height;
		glfwGetFramebufferSize(mpWindow, &width, &height);
		glViewport(0, 0, width, height);
		glfwSwapInterval(1);

		std::string modelBasePath = "data/cube/";
		Assimp::Importer importer;
		const struct aiScene* pScene = 
			importer.ReadFile(modelBasePath + "cube.obj", aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FixInfacingNormals);

		if (!pScene) {
			fprintf(stderr, "Failed to import scene: %s", importer.GetErrorString());
			glfwTerminate();
			return -1;
		}

		Texture::setBasePath(modelBasePath);
		Texture::setDefaultTexture(Texture::load("textures/white.png"));

		mGPUProgram.compileShader("data/basic.vert", ShaderType::VERTEX);
		mGPUProgram.compileShader("data/basic.frag", ShaderType::FRAGMENT);		
		mGPUProgram.link();
		std::cout << "Shader compilation log: " << mGPUProgram.getLog() << std::endl;
		mGPUProgram.printActiveAttribs();
		mGPUProgram.printActiveUniforms();

		processSceneNode(pScene, pScene->mRootNode);

		mCamera.setInput(Input(mpWindow));
		mCamera.setFieldOfView(45.0f);
		mCamera.setAspectRatio(static_cast<float>(width) / height);
		mCamera.setNearPlaneDistance(0.1f);
		mCamera.setFarPlaneDistance(10000.f);		
		mCamera.setPosition(0.0f, 0.0f, 100.0f);
		mCamera.setMovementRate(0.001f);
		mCamera.initialize();
		mRenderer.getRenderContext().pCamera = &mCamera;

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		glFrontFace(GL_CCW);
		glCullFace(GL_BACK);

		double totalTime = glfwGetTime();
		double elapsedTime = 0.0;

		while (!glfwWindowShouldClose(mpWindow)) {

			glfwPollEvents();

			mCamera.update(elapsedTime);

			mRenderer.getRenderContext().Time = static_cast<float>(totalTime);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			renderSceneNode(pScene, pScene->mRootNode);
			renderSceneNodeDebug(pScene, pScene->mRootNode);
			glfwSwapBuffers(mpWindow);

			const double currentTime = glfwGetTime();
			elapsedTime = (currentTime - totalTime) * 1000.0;

			totalTime = currentTime;

			//std::cout << "Elapsed time: " << elapsedTime << std::endl;
		}

		Texture::unloadAll();
		destroyMeshes();
		glfwTerminate();
		return 0;
	}
};

GLTest GLTest::sTheApp;

int main()
{
	return GLTest::sTheApp.run();
}
