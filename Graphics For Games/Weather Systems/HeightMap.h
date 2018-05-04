#pragma once

#include <fstream>
#include <iostream>
#include <string>

#include "..\nclgl\Mesh.h"

#define RAW_WIDTH 257
#define RAW_HEIGHT 257

#define HEIGHTMAP_X 16.0f
#define HEIGHTMAP_Z 16.0f
#define HEIGHTMAP_Y 1.25f
#define HEIGHTMAP_TEX_X 1.0f / 16.0f
#define HEIGHTMAP_TEX_Z 1.0f / 16.0f

class HeightMap : public Mesh
{
public:
	HeightMap(std::string name);
	~HeightMap();

	bool getPosition(float x, float z);

	void SetSnowTexture(GLuint tex) { snowTex = tex; }
	GLuint GetSnowTexture() { return snowTex; }

	void SetRainTexture(GLuint tex) { rainTex = tex; }
	GLuint GetRainTexture() { return rainTex; }

	void SetDefaultTexture() { defaultTex = texture; }

	virtual void Draw();
	void update(float msec);
	void reset();

protected:
	GLuint snowTex;
	GLuint rainTex;
	GLuint defaultTex;
	float* slopes;
	Vector3* snowVertex;
	bool* snowVertexPull;
};
