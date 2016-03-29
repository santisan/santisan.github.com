#pragma once
#include <string>
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <vector>

enum class ShaderType {
	VERTEX,
	FRAGMENT,
	GEOMETRY,
	TESS_CONTROL,
	TESS_EVALUATION
};

class GPUProgram
{
public:
	GPUProgram();
	~GPUProgram();

	bool compileShader(const char* fileName, ShaderType type);
	bool link();
	void use() const;
	std::string getLog() const;
	GLuint getHandle() const;
	bool isLinked() const;
	void bindAttribLocation(GLuint location, const char* name) const;
	void bindFragDataLocation(GLuint location, const char* name) const;
	void setUniform(const char* name, const glm::vec2& v) const;
	void setUniform(const char* name, const glm::vec3& v) const;
	void setUniform(const char* name, const glm::vec4& v) const;
	void setUniform(const char* name, const glm::mat3& m) const;
	void setUniform(const char* name, const glm::mat4& m) const;
	void setUniform(const char* name, const float val) const;
	void setUniform(const char* name, const int val) const;
	void setUniform(const char* name, const bool val) const;
	void printActiveUniforms() const;
	void printActiveAttribs() const;

private:
	GLuint mHandle;
	bool mLinked;
	std::string mLogString;
	std::vector<GLuint> mShaderHandles;

	GLint getUniformLocation(const char* name) const;
};

