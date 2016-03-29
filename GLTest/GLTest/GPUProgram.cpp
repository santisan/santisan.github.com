#include "GPUProgram.h"
#include <fstream>
#include <assert.h>

std::string loadShaderAsString(const char* fileName)
{
	std::string line, result;
	std::ifstream file;
	file.open(fileName);

	if (file.is_open()) {
		while(!file.eof()) {
			std::getline(file, line);
			result += line + '\n';
		}
	}

	file.close();
	return result;
}

GPUProgram::GPUProgram() :
	mHandle(0),
	mLinked(false)
{
}

GPUProgram::~GPUProgram()
{
	if (mHandle) {
		glDeleteProgram(mHandle);
		mHandle = 0;
	}
}

bool GPUProgram::compileShader(const char* fileName, ShaderType type)
{
	assert(fileName);
	if (!mHandle) {
		mHandle = glCreateProgram();
		if (!mHandle) {
			mLogString = "Error creating program";
			return false;
		}
	}

	GLenum shaderType = GL_VERTEX_SHADER;
	switch (type) {
		case ShaderType::VERTEX:
			shaderType = GL_VERTEX_SHADER;
			break;
		case ShaderType::FRAGMENT:
			shaderType = GL_FRAGMENT_SHADER;
			break;
		case ShaderType::GEOMETRY:
			shaderType = GL_GEOMETRY_SHADER;
			break;
		case ShaderType::TESS_CONTROL:
			shaderType = GL_TESS_CONTROL_SHADER;
			break;
		case ShaderType::TESS_EVALUATION:
			shaderType = GL_TESS_EVALUATION_SHADER;
			break;
	};
	GLuint shader = glCreateShader(shaderType);
	if (!shader) {
		mLogString = "Error creating shader";
		return false;
	}

	const std::string shaderCode = loadShaderAsString(fileName);
	const GLchar* const codeArray[] = { shaderCode.c_str() };
	glShaderSource(shader, 1, codeArray, nullptr);
	glCompileShader(shader);

	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result) {
		mLogString = "Shader compilation failed";
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if(logLen > 0) {
			char* log = new char[logLen];
			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, log);
			mLogString.append(": ");
			mLogString.append(log);
			delete[] log;
		}
		return false;
	}

	glAttachShader(mHandle, shader);
	mShaderHandles.push_back(shader);
	return true;
}

bool GPUProgram::link()
{
	assert(mHandle);
	glLinkProgram(mHandle);
	GLint status;
	glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
	mLinked = status != 0;
	bool result = false;
	if (!status) {
		mLogString = "Failed to link shader program";
		GLint logLen;
		glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char * log = new char[logLen];
			GLsizei written;
			glGetProgramInfoLog(mHandle, logLen, &written, log);
			mLogString.append(": ");
			mLogString.append(log);
			delete[] log;
		}
	}
	
	for (GLuint shader : mShaderHandles) {
		assert(shader);
		glDetachShader(mHandle, shader);
		glDeleteShader(shader);
	}
	mShaderHandles.clear();

	result = true;
	return result;
}

void GPUProgram::use() const
{
	assert(mHandle);
	glUseProgram(mHandle);
}

std::string GPUProgram::getLog() const
{
	return mLogString;
}

GLuint GPUProgram::getHandle() const
{
	return mHandle;
}

bool GPUProgram::isLinked() const
{
	return mLinked;
}

void GPUProgram::bindAttribLocation(GLuint location, const char* name) const
{
	assert(mHandle);
	assert(name);
	glBindAttribLocation(mHandle, location, name);
}

void GPUProgram::bindFragDataLocation(GLuint location, const char* name) const
{
	assert(mHandle);
	assert(name);
	glBindFragDataLocation(mHandle, location, name);
}

void GPUProgram::setUniform(const char* name, const glm::vec2& v) const
{
	GLint location = getUniformLocation(name);
	glUniform2f(location, v.x, v.y);
}

void GPUProgram::setUniform(const char* name, const glm::vec3& v) const
{
	GLint location = getUniformLocation(name);
	glUniform3f(location, v.x, v.y, v.z);
}

void GPUProgram::setUniform(const char* name, const glm::vec4& v) const
{
	GLint location = getUniformLocation(name);
	glUniform4f(location, v.x, v.y, v.z, v.w);
}

void GPUProgram::setUniform(const char* name, const glm::mat3& m) const	
{
	GLint location = getUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, &m[0][0]);
}

void GPUProgram::setUniform(const char* name, const glm::mat4& m) const
{
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
}

void GPUProgram::setUniform(const char* name, const float val) const
{
	GLint location = getUniformLocation(name);
	glUniform1f(location, val);
}

void GPUProgram::setUniform(const char* name, const int val) const
{
	GLint location = getUniformLocation(name);
	glUniform1i(location, val);
}

void GPUProgram::setUniform(const char* name, const bool val) const
{
	GLint location = getUniformLocation(name);
	glUniform1i(location, static_cast<GLint>(val));
}

void GPUProgram::printActiveUniforms() const
{
	assert(mHandle);
	GLint nUniforms, maxLen;
	glGetProgramiv(mHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxLen); 
	glGetProgramiv(mHandle, GL_ACTIVE_UNIFORMS, &nUniforms);
	GLchar* name = new GLchar[maxLen];
	GLint size, location;
	GLsizei written;
	GLenum type;
	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nUniforms; ++i) {
		glGetActiveUniform(mHandle, i, maxLen, &written, &size, &type, name);
		location = glGetUniformLocation(mHandle, name);
		printf(" %-8d | %s\n", location, name);
	}
	delete[] name;
}

void GPUProgram::printActiveAttribs() const
{
	assert(mHandle);
	GLint nUniforms, maxLen;
	glGetProgramiv(mHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxLen);
	glGetProgramiv(mHandle, GL_ACTIVE_ATTRIBUTES, &nUniforms);
	GLchar* name = new GLchar[maxLen];
	GLint size, location;
	GLsizei written;
	GLenum type;
	printf(" Location | Name\n");
	printf("------------------------------------------------\n");
	for (int i = 0; i < nUniforms; ++i) {
		glGetActiveAttrib(mHandle, i, maxLen, &written, &size, &type, name);
		location = glGetAttribLocation(mHandle, name);
		printf(" %-8d | %s\n", location, name);
	}
	delete[] name;
}

GLint GPUProgram::getUniformLocation(const char* name) const
{
	assert(mHandle);
	assert(name);
	return glGetUniformLocation(mHandle, name);
}
