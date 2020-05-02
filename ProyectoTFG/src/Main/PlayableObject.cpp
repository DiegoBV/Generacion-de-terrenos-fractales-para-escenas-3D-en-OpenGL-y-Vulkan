#include "PlayableObject.h"
#include "TimeManager.h"

PlayableObject::PlayableObject() : gravityDirection({ 0.0f, -1.0, 0.0f }), gravityForce(0.8), acceleration(5.0f)
{
	ssbo.position = { 0.0f, 0.75f, 50.0f };
	ssbo.velocity = { 0.0f, 0.0f, 0.0f };
	ssbo.mass = 1.0f;
	ssbo.damping = 0.1f;
}

PlayableObject::PlayableObject(glm::vec3 gravityDir, glm::vec3 velocity, glm::vec3 position, float gravityForce, float mass, float acceleration, float damping)
	: gravityDirection(gravityDir), gravityForce(gravityForce), acceleration(acceleration)
{
	ssbo.position = position;
	ssbo.velocity = velocity;
	ssbo.mass = mass;
	ssbo.damping = damping;
}

void PlayableObject::handleMovement(const char& key, const glm::vec3& direction)
{
	if (!ssbo.isGrounded) return;

	switch (key) {
	case 'W'://si pulsamos "w" acercamos la camara
		glm::vec3 f;
		f.z = direction.z;
		f.x = direction.x;
		f.y = 0.0f;
		f = normalize(f) * acceleration;
		addForce(f);
		break;
	case 'S'://"s" la alejamos
		f.z = -direction.z;
		f.x = -direction.x;
		f.y = 0.0f;
		f = normalize(f) * acceleration;
		addForce(f);
		break;
	case 'D'://derecha
		f.x = -direction.z;
		f.z = direction.x;
		f.y = 0.0f;
		f = normalize(f) * acceleration;
		addForce(f);
		break;
	case 'A'://izquierda
		f.x = direction.z;
		f.z = -direction.x;
		f.y = 0.0f;
		f = normalize(f) * acceleration;
		addForce(f);
		break;
	case 'Q':
		f.y = 1.0f;
		f.z = 0.0f;
		f.x = 0.0f;
		f = normalize(f) * acceleration;
		addForce(f);
		break;
	case 'E':
		f.y = -1.0f;
		f.z = 0.0f;
		f.x = 0.0f;
		f = normalize(f) * acceleration;
		addForce(f);
		break;
	default:
		break;
	}//switch
}

void PlayableObject::update(float deltaTime)
{
	addForce(gravityDirection * gravityForce);
	ssbo.deltaTime = deltaTime;
	ssbo.time = TimeManager::GetSingleton()->getTimeSinceBeginning();
}
