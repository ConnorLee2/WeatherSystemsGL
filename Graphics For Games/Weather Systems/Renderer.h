#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/camera.h"

#include "HeightMap.h"
#include "ParticleSystem.h"

class Renderer : public OGLRenderer
{
public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float msec);

protected:
	Camera *camera;

	// Heightmap
	HeightMap* heightMap;
	Shader* heightMapShader;
	void RenderHeightMap();

	// Particles
	ParticleSystem* particle;
	Shader* particleShader;
	bool renderParticle;
	void RenderParticleSystem();
	void chooseWeather();
	void setParticleSystem(Vector4 colour, int number, float speed, const char* texture, int t);
	void modifyWindSpeed();

	// Skybox
	Mesh* skybox;
	Shader* skyboxShader;
	void RenderSkybox();
	void UpdateTime();
	GLuint cubeMap;
	GLuint cubeMap2;
	bool isDaytime;
	float time;
};