#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SENSITIVITY = 0.3f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
private:
    // Attributes
    float velocity;
    glm::vec3 eye;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;


    // Euler Angles
    float yaw;
    float pitch;

    // Camera options
    float mouseSensitivity;
    float zoom;

    float minDist;
    float lastTarget;

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors();
    // Applies the given direction to the current position (eye)
    void applyDirection(Camera_Movement direction, float deltaTime);

public:
    // Default Constructor
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 5.5f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH, float velocity = 0.2f);
    // Scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch, float velocity);

    // Processes input received from keyboard
    void handleMovement(char key);
    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void handleOrientation(float xoffset, float yoffset, bool constrainPitch = true);
    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void handleZoom(float yoffset);

    void pivotTarget(glm::vec3 target, float offset);

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();
    inline const glm::vec3 getEye() const { return eye; };
    inline const glm::vec3 getFront() const { return front; };
    inline const glm::vec3 getRight() const { return right; };
    inline const glm::vec3 getUp() const { return up; };
    inline const glm::vec3 getWorldUp() const { return worldUp; };
    inline const float getMouseSensitivity() const { return mouseSensitivity; };
    inline const float getZoom() const { return zoom; };
    inline const float getPitch() const { return pitch; };
    inline const float getYaw() const { return yaw; };
    inline const float getVelocity() const { return velocity; };

    inline void setEye(glm::vec3 value) { eye = value; };
    inline void setFront(glm::vec3 value) { front = value; };
    inline void setMouseSensitivity(float value) { mouseSensitivity = value; };
    inline void setZoom(float value) { zoom = value; };
    inline void setVelocity(float value) { velocity = value; };
};
#endif