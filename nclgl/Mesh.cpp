#include "Mesh.h"

Mesh::Mesh(void) {
	for (int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}
	glGenVertexArrays(1, &arrayObject);

	numVertices = 0;
	numIndices = 0;
	indices = NULL;
	normals = NULL;
	vertices = NULL;
	colours = NULL;
	type = GL_TRIANGLES;

	texture = 0;
	textureCoords = NULL;
}


Mesh::~Mesh()
{// deletes VAO and VBOs
	glDeleteVertexArrays(1, &arrayObject);
	glDeleteBuffers(MAX_BUFFER, bufferObject);
	glDeleteTextures(1, &texture);
	delete[]vertices;
	delete[]colours;
	delete[]textureCoords;
	delete[]indices;
	delete[]normals;
}

void Mesh::Draw()
{
	//glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(arrayObject);

	if (bufferObject[INDEX_BUFFER] > 0) {
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else {
		glDrawArrays(type, 0, numVertices);
	}

	glBindVertexArray(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh * Mesh::GenerateTriangle()
{
	Mesh*m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f, 0.5f, 0.0f);
	m->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f, 0.0f);
	m->textureCoords[1] = Vector2(1.0f, 1.0f);
	m->textureCoords[2] = Vector2(0.0f, 1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

	m->BufferData();
	return m;
}

Mesh * Mesh::GenerateQuad()
{
	Mesh*m = new Mesh();
	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices = new Vector3[m->numVertices];
	m->textureCoords = new Vector2[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	m->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = Vector3(-1.0f, 1.0f, 0.0f);
	m->vertices[2] = Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = Vector3(1.0f, 1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f, 1.0f);
	m->textureCoords[1] = Vector2(0.0f, 0.0f);
	m->textureCoords[2] = Vector2(1.0f, 1.0f);
	m->textureCoords[3] = Vector2(1.0f, 0.0f);

	for (int i = 0; i < 4; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	m->BufferData();
	return m;
}

void Mesh::GenerateNormals()
{
	if (!normals) {
		normals = new Vector3[numVertices];
	}

	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i] = Vector3();
	}

	if (indices) { // genereate per-vertex normals
		for (GLuint i = 0; i < numIndices; i += 3) {
			unsigned int a = indices[i];
			unsigned int b = indices[i + 1];
			unsigned int c = indices[i + 2];

			Vector3 normal = Vector3::Cross((vertices[b] - vertices[a]), (vertices[c] - vertices[a]));

			normals[a] += normal;
			normals[b] += normal;
			normals[c] += normal;
		}
	}
	else { // its just a list of triangles, so generate face normals
		for (GLuint i = 0; i < numVertices; i += 3) {
			Vector3 &a = vertices[i];
			Vector3 &b = vertices[i + 1];
			Vector3 &c = vertices[i + 2];

			Vector3 normal = Vector3::Cross(b - a, c - a);

			normals[i] += normal;
			normals[i + 1] += normal;
			normals[i + 2] += normal;
		}
	}

	/* Normalise all normals */
	for (GLuint i = 0; i < numVertices; ++i) {
		normals[i].Normalise();
	}
	
}

void Mesh::BufferData()
{
	glBindVertexArray(arrayObject);
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
		vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(VERTEX_BUFFER);
	if (textureCoords) {
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector2),
			textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}
	if (colours) { // just in case data has no colour attributes
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector4),
			colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}
	if (normals) {
		glGenBuffers(1, &bufferObject[NORMAL_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vector3),
			normals, GL_STATIC_DRAW);
		glVertexAttribPointer(NORMAL_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(NORMAL_BUFFER);
	}
	if (indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(GLuint),
			indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
}

void Mesh::RebufferData()
{
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vector3), (void*)vertices);

	if (textureCoords) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vector2), (void*)textureCoords);
	}

	if (colours) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vector4), (void*)colours);
	}

	if (normals) {
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[NORMAL_BUFFER]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, numVertices * sizeof(Vector3), (void*)normals);
	}

	if (indices) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, numVertices * sizeof(unsigned int), (void*)indices);
	}
}
