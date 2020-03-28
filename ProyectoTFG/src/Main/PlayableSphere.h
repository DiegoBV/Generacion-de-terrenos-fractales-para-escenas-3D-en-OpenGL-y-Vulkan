#pragma once
#include "PlayableObject.h"

class PlayableSphere : public PlayableObject
{
private:
	virtual void calculateHitboxPoints();
public:
	PlayableSphere();
	PlayableSphere(glm::vec3 gravity, glm::vec3 velocity, glm::vec3 position, float mass, float acceleration, float damping, float radius);

	//DEBUG?
	void setRadius(float rad) { ssbo.radius = rad; }
	float getRadius() { return ssbo.radius; }
	//DEBUG?
};

