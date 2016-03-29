#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBuffer
{
public:
	VertexBuffer() :
		VAO(0), VertexCount(0)
	{
	}

	void clear() 
	{
		VBOs.clear();
		VAO = 0;
		VertexCount = 0;
	}

	std::vector<GLuint> VBOs;
	GLuint VAO;
	unsigned int VertexCount;
};

struct IndexBuffer
{
public:
	IndexBuffer() :
		ElementBuffer(0),
		IndexCount(0)
	{
	}

	void clear()
	{
		ElementBuffer = 0;
		IndexCount = 0;
	}

	GLuint ElementBuffer;
	unsigned int IndexCount;
};
