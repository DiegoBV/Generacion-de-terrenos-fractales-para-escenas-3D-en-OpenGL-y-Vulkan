#pragma once
#include "PlayableObject.h"

class PlayableSphere : public PlayableObject
{
private:
	virtual void calculateHitboxPoints();
public:
	PlayableSphere() : PlayableObject() { calculateHitboxPoints(); };
	PlayableSphere(glm::vec3 gravity, float acceleration) : PlayableObject(gravity, acceleration) { calculateHitboxPoints(); };
};

