#include <iostream>
#include <cmath>
#include "render/Camera.h"

class Camera {
public:
    glm::vec3 position;       // Camera position
    glm::vec3 front;          // Direction the camera is facing
    glm::vec3 up;             // Up direction for the camera
    glm::vec3 right;          // Right direction for the camera
    glm::vec3 worldUp;        // World up vector

    float yaw;                // Rotation around Y-axis
    float pitch;              // Rotation around X-axis

    float speed;              // Speed at which the camera moves
    float sensitivity;        // Sensitivity of mouse movement for rotation

    Camera(glm::vec3 startPos, glm::vec3 startUp, float startYaw, float startPitch) 
        : position(startPos), worldUp(startUp), yaw(startYaw), pitch(startPitch) {
        front = glm::vec3(0.0f, 0.0f, -1.0f);
        updateCameraVectors();
        speed = 2.5f;
        sensitivity = 0.1f;
    }

    // Update camera vectors based on yaw and pitch
    void updateCameraVectors() {
        glm::vec3 frontTemp;
        frontTemp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        frontTemp.y = sin(glm::radians(pitch));
        frontTemp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(frontTemp);

        right = glm::normalize(glm::cross(front, worldUp));  // Normalize the right vector
        up = glm::normalize(glm::cross(right, front));       // Normalize the up vector
    }

    // Move the camera by updating its position
    void processKeyboardMovement(char direction, float deltaTime) {
        float velocity = speed * deltaTime;
        if (direction == 'W') {
            position += front * velocity;
        } else if (direction == 'S') {
            position -= front * velocity;
        } else if (direction == 'A') {
            position -= right * velocity;
        } else if (direction == 'D') {
            position += right * velocity;
        }
    }

    // Adjust the camera orientation based on mouse movement
    void processMouseMovement(float xOffset, float yOffset) {
        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch -= yOffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        updateCameraVectors();
    }

    // Get the view matrix for the camera
    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }
};
