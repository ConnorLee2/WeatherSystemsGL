#pragma once
#include "../../nclgl/Mesh.h"
#include "HeightMap.h"

struct Particle {
	Vector3 position;
	Vector4 colour;
	Vector3 velocity;
	float life;
};

class ParticleSystem : public Mesh
{
public:
	ParticleSystem();
	ParticleSystem(Vector4 colour, int number, float speed, HeightMap* heightMap, int t);
	~ParticleSystem();

	void initialiseParticles(Vector4 colour);
	virtual void Draw();
	virtual void update(float msec);

	float getBlend() { return blend; }
	void updateBlend();

	void moveParticle(Particle* particle);
	bool collision(Vector3 position);
	int getWeather() { return weather; }
	void modifyWindSpeed(Vector3 speed) { windSpeed = windSpeed + speed; }
	void resetWindSpeed() { windSpeed = Vector3(0, 0, 0); }

protected:
	vector<Particle*> particles;
	int numParticles;
	int heightMapLength;
	float particleSpeed;
	int weather;
	Vector3 windSpeed;
	float blend;
	HeightMap* heightMap;
};