#pragma once
#include "OGLRenderer.h"

enum MeshBuffer {
	VERTEX_BUFFER, COLOUR_BUFFER,
	TEXTURE_BUFFER, NORMAL_BUFFER, 
	INDEX_BUFFER, MAX_BUFFER
};

class Mesh
{
public:
	Mesh(void);
	~Mesh(void);

	virtual void Draw();
	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();

	void SetTexture(GLuint tex) { texture = tex; }
	GLuint GetTexture() { return texture; }

protected:
	void BufferData();
	void RebufferData();
	void GenerateNormals();

	GLuint arrayObject;
	GLuint bufferObject[MAX_BUFFER];
	GLuint numVertices;
	GLuint numIndices;
	unsigned int* indices;
	GLuint type;

	Vector3* vertices;
	Vector3* normals;
	Vector4* colours;

	GLuint texture;
	Vector2* textureCoords;
};

