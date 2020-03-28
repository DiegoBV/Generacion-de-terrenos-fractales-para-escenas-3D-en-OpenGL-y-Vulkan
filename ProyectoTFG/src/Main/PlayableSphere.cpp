#include "PlayableSphere.h"

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
}