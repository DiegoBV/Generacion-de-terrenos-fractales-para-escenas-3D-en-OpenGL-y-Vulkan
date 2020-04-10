#include "Camera.h"

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

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), eye(position), worldUp(up), yaw(yaw), pitch(pitch)
{
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) 
    : front(glm::vec3(0.0f, 0.0f, -1.0f)), movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM), yaw(yaw), pitch(pitch)
{
    eye = glm::vec3(posX, posY, posZ);
    worldUp = glm::vec3(upX, upY, upZ);
    updateCameraVectors();
}

void Camera::handleMovement(Camera_Movement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        eye += front * velocity;
    if (direction == BACKWARD)
        eye -= front * velocity;
    if (direction == LEFT)
        eye -= right * velocity;
    if (direction == RIGHT)
        eye += right * velocity;
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
    glm::vec3 aux = (target - (front * offset));
    //float auxY = eye.y;
    //eye = aux;
    //if (target.y >= eye.y + offset) {
    //    eye.y = aux.y;
    //}
    //else {
    //    eye.y = auxY;
    //}
    // if ((eye - (target - (front * offset))).y < 0.01f) return;
    eye = aux;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(eye, eye + front, worldUp);
}
