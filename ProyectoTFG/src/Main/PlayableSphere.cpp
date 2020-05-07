#include "PlayableSphere.h"

PlayableSphere::PlayableSphere() : PlayableObject()
{
	ssbo.radius = 0.05f;
	calculateHitboxPoints();
}

PlayableSphere::PlayableSphere(glm::vec3 gravityDirection, glm::vec3 velocity, glm::vec3 position, float gravityForce, float mass, 
	float acceleration, float damping, float airDamping,  float radius)
	: PlayableObject(gravityDirection, velocity, position, gravityForce, mass, acceleration, damping, airDamping)
{
	ssbo.radius = radius;
	calculateHitboxPoints();
}

void PlayableSphere::calculateHitboxPoints()
{
	// fibonacci sphere

	float rnd = ((float)rand() / (RAND_MAX)) + 1;

	float offset = 2.0 / COLLISION_SAMPLES;

	float increment = M_PI * (3.0 - sqrt(5.0));

	for (int i = 0; i < COLLISION_SAMPLES; i++) {
		float y = ((i * offset) - 1) + (offset / 2);
		float r = sqrt(1 - pow(y, 2));

		float phi = (int(i + rnd) % COLLISION_SAMPLES) * increment;

		float x = cos(phi) * r;
		float z = sin(phi) * r;

		hitboxPoints.push_back(glm::normalize(glm::vec3(x, y, z)));
	}

	std::copy(hitboxPoints.begin(), hitboxPoints.end(), ssbo.collisionDirs);
}
