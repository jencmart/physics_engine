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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include "object.hpp"

class Projectile : public Object{

public:

    Projectile(glm::vec3 position,
               glm::vec3 direction,
               std::vector<GLfloat> physicalCoords,
               std::vector<GLfloat> modelColors,
               bool triangles,
               unsigned long arraySize);
    bool move(float deltaTime) override;

    void updateRotation(float deltaTime);

    void updateScale(float deltaTime);

    void updateTranslation(float time);
};


#endif //OPENGL_GAME_PROJECTILE_HPP
