#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 up;
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

Camera(float width, float height) {
    position   = glm::vec3(0.0f, -0.5f, 7.0f);
    target     = glm::vec3(0.0f, -0.5f, 0.0f);
    up         = glm::vec3(0.0f, 1.0f, 0.0f);

    fov        = 35.0f;
    aspect     = width / height;
    nearPlane  = 0.1f;
    farPlane   = 100.0f;
}



    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 getProjectionMatrix() const {
        return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    }
};

/*  

Author: theurg1st  
Website: https://theurg1st.github.io

========================================
License
========================================

This project is released under the MIT License.  
You may use, modify, or redistribute the source code with attribution.

========================================
Credits
========================================

Made by theurg1st

*/