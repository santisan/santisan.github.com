#include "Renderer.h"
#include <assert.h>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"

void Renderer::render(const Mesh& mesh)
{
	const VertexBuffer& vertexBuffer = mesh.getVertexBuffer();
	const IndexBuffer& indexBuffer = mesh.getIndexBuffer();
	assert(vertexBuffer.VAO);
	assert(indexBuffer.ElementBuffer);

	mesh.getMaterial().apply(mRenderContext);

	glBindVertexArray(vertexBuffer.VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.ElementBuffer);
	glDrawElements(GL_TRIANGLES, indexBuffer.IndexCount, GL_UNSIGNED_INT, (const void*)0);
}

void Renderer::renderDebug(const Mesh& mesh)
{
	const std::vector<glm::vec3> vertices = mesh.getVertices();
	const std::vector<glm::vec3> normals = mesh.getNormals();
	const std::vector<glm::vec3> tangents = mesh.getTangents();
	const std::vector<glm::vec3> bitangents = mesh.getBitangents();

	glUseProgram(0);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((const GLfloat*)&mRenderContext.pCamera->getProjectionMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glm::mat4 worldView = mRenderContext.pCamera->getViewMatrix() * mRenderContext.getCurrentWorldMatrix();
	glLoadMatrixf((const GLfloat*)&worldView[0][0]);

	const float length = 0.2f;

	glColor3f(0, 0, 1);
	glBegin(GL_LINES);
	for (int i=0; i < vertices.size(); ++i) {
		glm::vec3 p = vertices[i];
		glVertex3fv(&p.x);
		const glm::vec3 o = glm::normalize(normals[i]);
		p += o * length;
		glVertex3fv(&p.x);
	}
	
	glColor3f(1, 0, 0);
	for (int i=0; i < vertices.size(); ++i) {
		glm::vec3 p = vertices[i];
		glVertex3fv(&p.x);
		const glm::vec3 o = glm::normalize(tangents[i]);
		p += o * length;
		glVertex3fv(&p.x);
	}
	
	glColor3f(0, 1, 0);
	for (int i=0; i < vertices.size(); ++i) {
		glm::vec3 p = vertices[i];
		glVertex3fv(&p.x);
		const glm::vec3 o = glm::normalize(bitangents[i]);
		p += o * length;
		glVertex3fv(&p.x);
	}
	glEnd();
}
