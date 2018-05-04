#include "ParticleSystem.h"


ParticleSystem::ParticleSystem()
{
}

ParticleSystem::ParticleSystem(Vector4 colour, int number, float speed, HeightMap* map, int t)
{
	// Set up variables
	numParticles = number;
	heightMapLength = HEIGHTMAP_X * RAW_WIDTH;
	particleSpeed = speed;
	initialiseParticles(colour);
	heightMap = map;
	weather = t;
	blend = 0.0f;
	windSpeed = Vector3(0, 0, 0);
}

ParticleSystem::~ParticleSystem()
{
	// Delete particles in list
	for (vector<Particle *>::iterator it = particles.begin(); it != particles.end(); ++it) {
		delete * it;
		it = particles.erase(it);
	}
	delete heightMap;
}

void ParticleSystem::initialiseParticles(Vector4 colour)
{
	// Initialise particles
	for (int i = 0; i < numParticles; i++) {
		Particle* temp = new Particle();
		// Particle position
		temp->position.x = rand() % heightMapLength;
		temp->position.y = 2000.0f;
		temp->position.z = rand() % heightMapLength;

		// Particle colour, lifetime and speed
		temp->colour = colour;
		temp->life = rand() % 15000 + 10000.0f;; // 1000ms = 1 second, 10 secs to 15 secs
		temp->velocity.x += ((rand() % 200 + (-100)) / 100.0f) * 0.5f;
		temp->velocity.y += -((rand() % 101) / 100.0f) * particleSpeed;
		temp->velocity.z += ((rand() % 200 + (-100)) / 100.0f) * 0.5f;
		particles.push_back(temp);
	}
	vertices = new Vector3[particles.size()];
	colours = new Vector4[particles.size()];

	type = GL_POINTS;
	numVertices = particles.size();

	for (unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i] = particles.at(i)->colour;
	}

	BufferData();
}

void ParticleSystem::Draw()
{
	// Set up vertex buffers	
	for (unsigned int i = 0; i < particles.size(); ++i) {
		vertices[i] = particles.at(i)->position;
		colours[i] = particles.at(i)->colour;
	}

	RebufferData();

	// Render particles
	Mesh::Draw();
}

void ParticleSystem::update(float msec)
{
	// Update particles
	for (vector<Particle *>::iterator it = particles.begin(); it != particles.end();) {
		// Get particle and reduce lifetime
		Particle* p = (*it);
		p->life -= msec;

		// If particle is past 0.0f, check its location then output a new particle
		if (p->position.y <= 0.0f) {
			// snow
			if (weather == 1) {
				if (collision(p->position)) {
					p->position = Vector3(rand() % heightMapLength, 2000.0f, rand() % heightMapLength);
					p->life = rand() % 15000 + 10000.0f;
					moveParticle(p);
					updateBlend();
				}
			}
			// rain, other
			else {
				p->position = Vector3(rand() % heightMapLength, 2000.0f, rand() % heightMapLength);
				p->life = rand() % 15000 + 10000.0f;
				moveParticle(p);
				updateBlend();
			}
		}

		// life is 0
		if (p->life < 0.0f) {
			// reset position, life and immediately re-emit particle
			p->position = Vector3(rand() % heightMapLength, 2000.0f, rand() % heightMapLength);
			p->life = rand() % 15000 + 10000.0f;
			moveParticle(p);
		}

		// else move particle
		else {
			moveParticle(p);
			++it;
		}
	}
}

void ParticleSystem::updateBlend()
{
	if (blend <= 1.0f) {
		blend = blend + 0.0005f;
	}
	else {
		blend = 1.0f;
	}
}

void ParticleSystem::moveParticle(Particle * particle)
{
	particle->position += particle->velocity + windSpeed;
}

bool ParticleSystem::collision(Vector3 position)
{
	float x = position.x;
	float z = position.z;
	return heightMap->getPosition(x, z);
}