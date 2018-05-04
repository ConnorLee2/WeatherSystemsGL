#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	// Create objects
	camera = new Camera(0.0f, 0.0f, Vector3(RAW_WIDTH * HEIGHTMAP_X / 2.0f, 500, RAW_HEIGHT * HEIGHTMAP_Z));
	heightMap = new HeightMap(TEXTUREDIR "terrain.raw");
	skybox = Mesh::GenerateQuad();

	// Set shaders
	heightMapShader = new Shader(SHADERDIR "HeightMapVertex.glsl", SHADERDIR "HeightMapFragment.glsl");
	particleShader = new Shader(SHADERDIR "particleVertex.glsl", SHADERDIR "particleFrag.glsl", SHADERDIR "particleGeom.glsl");
	skyboxShader = new Shader(SHADERDIR "skyboxVertex.glsl", SHADERDIR "skyboxFragment.glsl");

	// Projection matrix
	projMatrix = Matrix4::Perspective(1.0f, 15000.0f, (float)width / (float)height, 45.0f);

	// Link shader programs
	if (!heightMapShader->LinkProgram() || !particleShader->LinkProgram() ||
		!skyboxShader->LinkProgram()) {
		return;
	}

	// Set heightmap texture
	heightMap->SetTexture(SOIL_load_OGL_texture(TEXTUREDIR "ground.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!heightMap->GetTexture())
	{
		return;
	}
	SetTextureRepeating(heightMap->GetTexture(), true);
	heightMap->SetDefaultTexture();

	// Set snow texture
	heightMap->SetSnowTexture(SOIL_load_OGL_texture(TEXTUREDIR "snow.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!heightMap->GetSnowTexture())
	{
		return;
	}
	SetTextureRepeating(heightMap->GetSnowTexture(), true);

	// Set wet texture
	heightMap->SetRainTexture(SOIL_load_OGL_texture(TEXTUREDIR "groundwet.jpg",
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS));
	if (!heightMap->GetRainTexture())
	{
		return;
	}
	SetTextureRepeating(heightMap->GetRainTexture(), true);

	// Set skybox textures
	cubeMap = (SOIL_load_OGL_cubemap(
		TEXTUREDIR "skybox/SkyNoon_Left.png", TEXTUREDIR "skybox/SkyNoon_Right.png",
		TEXTUREDIR "skybox/SkyNoon_Top.png", TEXTUREDIR "skybox/SkyNoon_Bottom.png",
		TEXTUREDIR "skybox/SkyNoon_Back.png", TEXTUREDIR "skybox/SkyNoon_Front.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	));
	cubeMap2 = (SOIL_load_OGL_cubemap(
		TEXTUREDIR "skybox/SkyNight_Left.png", TEXTUREDIR "skybox/SkyNight_Right.png",
		TEXTUREDIR "skybox/SkyNight_Top.png", TEXTUREDIR "skybox/SkyNight_Bottom.png",
		TEXTUREDIR "skybox/SkyNight_Back.png", TEXTUREDIR "skybox/SkyNight_Front.png",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID, 0
	));
	if (!cubeMap || !cubeMap2) {
		return;
	}

	// Initialise variables
	isDaytime = true;
	time = 0.0f;
	renderParticle = false;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	init = true;
}

Renderer::~Renderer() {
	// Delete objects
	delete camera;
	delete heightMap;
	delete skybox;
	if (renderParticle) {
		delete particle;
	}

	// Delete shaders
	delete heightMapShader;
	delete particleShader;
	currentShader = 0;
}

void Renderer::UpdateScene(float msec) {
	camera->UpdateCamera(msec);

	if (renderParticle)
	{
		modifyWindSpeed();
		particle->update(msec);
		heightMap->update(msec);
	}

	viewMatrix = camera->BuildViewMatrix();
	UpdateTime();
}

void Renderer::RenderScene() {
	// Clear background colour
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene
	RenderSkybox();
	RenderHeightMap();
	chooseWeather();

	if (renderParticle)
	{
		RenderParticleSystem();
	}
	SwapBuffers();
}

void Renderer::RenderParticleSystem() {
	// Enabled to allow particles to draw over each other and blend properly
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	// Set shader and uniforms
	SetCurrentShader(particleShader);
	glUseProgram(currentShader->GetProgram());
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "size"), 50.0f);

	// Set texture
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, particle->GetTexture());

	UpdateShaderMatrices();

	// Render particles
	particle->Draw();

	glUseProgram(0);

	// Reenable depth testing and disable blending
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Disable textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::chooseWeather()
{
	// If 1 is pressed, render snow
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_1))
	{
		// default 3000
		setParticleSystem(Vector4(1.0, 1.0, 1.0, 1.0f), 3000, 6.0f, TEXTUREDIR"particle.tga", 1);
		heightMap->reset();
	}

	// If 2 is pressed, render rain
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_2))
	{
		// default 2000
		setParticleSystem(Vector4(0.529, 0.808, 0.922, 1.0), 2000, 10.0f, TEXTUREDIR"rain.tga", 2);
		heightMap->reset();
	}

	// if 3 is pressed, render nothing
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_3))
	{
		renderParticle = false;
	}
}

void Renderer::setParticleSystem(Vector4 colour, int number, float speed, const char* texture, int t)
{
	// Colour, number of particles
	particle = new ParticleSystem(colour, number, speed, heightMap, t);

	// Texture
	particle->SetTexture(SOIL_load_OGL_texture(texture,
		SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_COMPRESS_TO_DXT));
	if (!particle->GetTexture())
	{
		return;
	}

	renderParticle = true;
}

void Renderer::modifyWindSpeed()
{
	// If 6 is pressed, increase x wind speed
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_6))
	{
		particle->modifyWindSpeed(Vector3(1, 0, 0));
	}

	// If 7 is pressed, decrease x wind speed
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_7))
	{
		particle->modifyWindSpeed(Vector3(-1, 0, 0));
	}

	// if 8 is pressed, increase z wind speed
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_8))
	{
		particle->modifyWindSpeed(Vector3(0, 0, 1));
	}

	// if 9 is pressed, decrease z wind speed
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_9))
	{
		particle->modifyWindSpeed(Vector3(0, 0, -1));
	}

	// if 0 is pressed, reset wind speed to 0
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_0))
	{
		particle->resetWindSpeed();
	}
}

void Renderer::RenderSkybox()
{
	// Enable blending and disable depth mask
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);

	// Set shader
	SetCurrentShader(skyboxShader);
	glUseProgram(currentShader->GetProgram());

	// Set day/night textures and time
	glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "time"), time);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "cubeTex2"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap2);

	UpdateShaderMatrices();

	// Render skybox
	skybox->Draw();

	glUseProgram(0);

	// Reenable depth testing and disable blending
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

	// Disable textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::UpdateTime()
{
	if (isDaytime)
	{
		time += 0.0005f;
		if (time >= 1.0f)
		{
			isDaytime = false;
		}
	}
	else
	{
		time -= 0.0005f;
		if (time <= 0.0f)
		{
			isDaytime = true;
		}
	}
}

void Renderer::RenderHeightMap() {
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// Set shader
	SetCurrentShader(heightMapShader);
	glUseProgram(currentShader->GetProgram());

	// Set texture one
	glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, heightMap->GetTexture());

	// Set texture two and blend factor
	if (renderParticle) {
		glUniform1f(glGetUniformLocation(currentShader->GetProgram(), "blend"), particle->getBlend());
		glUniform1i(glGetUniformLocation(currentShader->GetProgram(), "diffuseTex2"), 1);
		glActiveTexture(GL_TEXTURE1);
		if (particle->getWeather() == 1) {
			glBindTexture(GL_TEXTURE_2D, heightMap->GetSnowTexture());
		}
		else if (particle->getWeather() == 2) {
			glBindTexture(GL_TEXTURE_2D, heightMap->GetRainTexture());
		}
	}
	else {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, heightMap->GetTexture());
	}

	UpdateShaderMatrices();

	// Render heightmap
	heightMap->Draw();

	glUseProgram(0);

	// Disable textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0);
}