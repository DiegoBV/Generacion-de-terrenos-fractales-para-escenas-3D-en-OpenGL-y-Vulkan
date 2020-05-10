#pragma once
#include "ShaderUtils.h"
#include <vector>

class PlayableObject
{
protected:
	StorageBufferObject ssbo = {};
	float acceleration;
	float fractalVelocity = 0.05f;
	glm::vec3 keyDirection;
	glm::vec3 gravityDirection;
	float gravityForce;
	std::vector<glm::vec3> hitboxPoints;
	virtual void calculateHitboxPoints() = 0;

public:
	PlayableObject();
	PlayableObject(glm::vec3 gravityDir, glm::vec3 velocity, glm::vec3 position, float gravityForce, float mass, 
		float acceleration, float damping, float airDamping);
	~PlayableObject() {};

	void handleMovement(const char& key, const glm::vec3& dir, bool terrain);
	void update(float deltaTime);

	inline void addForce(glm::vec3 value) { ssbo.force += value; };
	inline void resetKeyDirection() { keyDirection = { 0.0f, 0.0f, 0.0f }; };
	inline StorageBufferObject getSSBO() { return ssbo; };
	inline float getAcceleration() { return acceleration; }
	inline glm::vec3 getPosition() { return ssbo.position; }
	inline glm::vec3 getDirection() { return keyDirection; }
	inline std::vector<glm::vec3> getHitboxPoints() { return hitboxPoints; };

	inline void setGravityDirection(glm::vec3 value) { gravityDirection = value; }
	inline void setSSBO(const StorageBufferObject& value) { ssbo = value; };
};

