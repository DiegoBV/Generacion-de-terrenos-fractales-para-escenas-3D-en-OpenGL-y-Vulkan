#pragma once
#include "ShaderUtils.h"
#include <vector>

class PlayableObject
{
protected:
	StorageBufferObject ssbo = {};
	float acceleration;
	glm::vec3 gravityDirection;
	float gravityForce;
	std::vector<glm::vec3> hitboxPoints;
	virtual void calculateHitboxPoints() = 0;

public:
	PlayableObject();
	PlayableObject(glm::vec3 gravityDir, glm::vec3 velocity, glm::vec3 position, float gravityForce, float mass, float acceleration, float damping);
	~PlayableObject() {};

	glm::vec3 getMovementDirection(const char& key, const glm::vec3& direction);
	void update(float deltaTime);

	inline void addForce(glm::vec3 value) { ssbo.force += value; };
	inline StorageBufferObject getSSBO() { return ssbo; };
	inline float getAcceleration() { return acceleration; }
	inline glm::vec3 getPosition() { return ssbo.position; }
	inline std::vector<glm::vec3> getHitboxPoints() { return hitboxPoints; };

	inline void setGravityDirection(glm::vec3 value) { gravityDirection = value; }
	inline void setSSBO(const StorageBufferObject& value) { ssbo = value; };
};

