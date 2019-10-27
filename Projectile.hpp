//
// Created by jencmart on 10/27/19.
//

#ifndef OPENGL_GAME_PROJECTILE_HPP
#define OPENGL_GAME_PROJECTILE_HPP

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic. ? MAGIC ?
#include <GL/glew.h>

// Include GLFW  - handle window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - opengl mathematics
#include <glm/glm.hpp>
#include <vector>
#include "object.hpp"

class Projectile : public Object{

public:
    glm::vec3 direction; // for updating model matrix
    glm::vec3 position; // for updating model matrix
    float speed;         // for updating model matrix
    float age;

    Projectile(glm::vec3 position,
               glm::vec3 direction,
               std::vector<GLfloat> physicalCoords,
               std::vector<GLfloat> modelColors,
               bool triangles,
               unsigned long arraySize);
    bool move(float deltaTime) override;

    void updateRotation(float deltaTime);
};


#endif //OPENGL_GAME_PROJECTILE_HPP
