#include "Camera.h"
#include "TimeManager.h"

void Camera::updateCameraVectors()
{
    // Calculate the new Front vector
    glm::vec3 auxfront;
    auxfront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    auxfront.y = sin(glm::radians(pitch));
    auxfront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(auxfront);
    // Also re-calculate the Right and Up vector
    right = glm::normalize(glm::cross(front, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up = glm::normalize(glm::cross(right, front));
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float velocity)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), velocity(velocity), mouseSensitivity(SENSITIVITY), zoom(ZOOM), eye(position), worldUp(up), 
    yaw(yaw), pitch(pitch), lastTarget(0.0f), minDist(0.005)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float velocity) 
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), velocity(velocity), mouseSensitivity(SENSITIVITY), zoom(ZOOM),
    yaw(yaw), pitch(pitch), lastTarget( 0.0f), minDist(0.005)
{
    eye = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    updateCameraVectors();
}

void Camera::applyDirection(Camera_Movement direction, float deltaTime)
{
    float vel = velocity * deltaTime;
    if (direction == FORWARD)
        eye += front * vel;
    if (direction == BACKWARD)
        eye -= front * vel;
    if (direction == LEFT)
        eye -= right * vel;
    if (direction == RIGHT)
        eye += right * vel;
}

void Camera::handleMovement(char key)
{
    double deltaTime = TimeManager::GetSingleton()->getDeltaTime();

    switch (key) {
    case 'W'://si pulsamos "w" acercamos la camara
        applyDirection(Camera_Movement::FORWARD, deltaTime);
        break;
    case 'S'://"s" la alejamos
        applyDirection(Camera_Movement::BACKWARD, deltaTime);
        break;
    case 'D'://derecha
        applyDirection(Camera_Movement::RIGHT, deltaTime);
        break;
    case 'A'://izquierda
        applyDirection(Camera_Movement::LEFT, deltaTime);
        break;
    default:
        break;
    }//switch
}

void Camera::handleOrientation(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Neck
    if (constrainPitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

void Camera::handleZoom(float yoffset)
{
    if (zoom >= 1.0f && zoom <= 45.0f)
        zoom -= yoffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 45.0f)
        zoom = 45.0f;
}

void Camera::pivotTarget(glm::vec3 target, float offset)
{
    if (glm::distance(target.y, lastTarget) > minDist) {
        lastTarget = target.y;
    }
    target.y = lastTarget;
    glm::vec3 aux = (target - (front * offset));
    eye = aux;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(eye, eye + front, worldUp);
}
