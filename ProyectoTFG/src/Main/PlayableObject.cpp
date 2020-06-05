#include "PlayableObject.h"
#include "TimeManager.h"

PlayableObject::PlayableObject() : gravityDirection({ 0.0f, -1.0, 0.0f }), keyDirection({ 0.0f, 0.0f, 0.0f }), gravityForce(0.8), acceleration(5.0f)
{
	ssbo.position = { 0.0f, 0.75f, 50.0f };
	ssbo.velocity = { 0.0f, 0.0f, 0.0f };
	ssbo.mass = 1.0f;
	ssbo.damping = 0.1f;
	ssbo.airDamping = 0.5f;
}

PlayableObject::PlayableObject(glm::vec3 gravityDir, glm::vec3 velocity, glm::vec3 position, float gravityForce, float mass, 
	float acceleration, float damping, float airDamping)
	: gravityDirection(gravityDir), keyDirection({ 0, 0, 0 }), gravityForce(gravityForce), acceleration(acceleration)
{
	ssbo.position = position;
	ssbo.velocity = velocity;
	ssbo.mass = mass;
	ssbo.damping = damping;
	ssbo.airDamping = airDamping;
}

void PlayableObject::handleMovement(const char& key, const glm::vec3& direction, bool terrain)
{
	if (!ssbo.isGrounded) return;

	glm::vec3 d;
	switch (key) {
	case 'W'://si pulsamos "w" acercamos la camara
		d.z = direction.z;
		d.x = direction.x;
		d.y = 0.0f;	
		keyDirection = { 0.0f, 0.0f, 1.0f };
		break;
	case 'S'://"s" la alejamos
		d.z = -direction.z;
		d.x = -direction.x;
		d.y = 0.0f;
		keyDirection = { 0.0f, 0.0f, -1.0f };
		break;
	case 'D'://derecha
		d.x = -direction.z;
		d.z = direction.x;
		d.y = 0.0f;
		keyDirection = { 1.0f, 0.0f, 0.0f };
		break;
	case 'A'://izquierda
		d.x = direction.z;
		d.z = -direction.x;
		d.y = 0.0f;
		keyDirection = { -1.0f, 0.0f, 0.0f };
		break;
	case 'Q':
		d.y = 1.0f;
		d.z = 0.0f;
		d.x = 0.0f;
		break;
	case 'E':
		d.y = -1.0f;
		d.z = 0.0f;
		d.x = 0.0f;
		break;
	default:
		break;
	}//switch

	d = glm::normalize(d);
	if (terrain) addForce(d * acceleration);
	else ssbo.fractalRotation += d * fractalVelocity;
}

void PlayableObject::update(float deltaTime)
{
	if(!ssbo.isGrounded)
		addForce(gravityDirection * gravityForce);

	ssbo.deltaTime = deltaTime;
	ssbo.time = TimeManager::GetSingleton()->getTimeSinceBeginning();
}
