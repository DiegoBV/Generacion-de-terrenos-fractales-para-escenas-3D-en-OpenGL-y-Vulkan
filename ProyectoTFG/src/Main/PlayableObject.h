#pragma once
#include "ShaderUtils.h"
#include <vector>

class PlayableObject
{
protected:
	StorageBufferObject ssbo = {};
	float acceleration;
	glm::vec3 gravity;
	std::vector<glm::vec3> hitboxPoints;
	virtual void calculateHitboxPoints() = 0;

public:
	PlayableObject();
	PlayableObject(glm::vec3 gravity, glm::vec3 velocity, glm::vec3 position, float mass, float acceleration, float damping);
	~PlayableObject() {};

	void handleMovement(const char& key, const glm::vec3& direction);
	void update(float deltaTime);

	inline void addForce(glm::vec3 value) { ssbo.force += value; };
	inline StorageBufferObject getSSBO() { return ssbo; };
	inline std::vector<glm::vec3> getHitboxPoints() { return hitboxPoints; };

	inline void setSSBO(const StorageBufferObject& value) { ssbo = value; };
};

