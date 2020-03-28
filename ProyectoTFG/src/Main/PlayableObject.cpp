#include "PlayableObject.h"

void PlayableObject::handleMovement(const char& key, const glm::vec3& direction)
{
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
	addForce(gravity);
	ssbo.deltaTime = deltaTime;
}
