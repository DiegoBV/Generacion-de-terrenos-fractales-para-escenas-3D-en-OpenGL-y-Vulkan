#pragma once
#include "PlayableObject.h"

class PlayableSphere : public PlayableObject
{
private:
	virtual void calculateHitboxPoints();
public:
	PlayableSphere();
	PlayableSphere(glm::vec3 gravityDirection, glm::vec3 velocity, glm::vec3 position, float gravityForce, float mass, 
		float acceleration, float damping, float airDamping, float radius);

	//DEBUG?
	void setRadius(float rad) { ssbo.radius = rad; }
	float getRadius() { return ssbo.radius; }
	//DEBUG?
};

