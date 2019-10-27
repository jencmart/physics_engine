//
// Created by jencmart on 10/27/19.
//

#include "Projectile.hpp"

/// ---------------------------- PROJECTILE

Projectile::Projectile(glm::vec3 position,
                       glm::vec3 direction,
                       std::vector<GLfloat> physicalCoords,
                       std::vector<GLfloat> modelColors,
                       bool triangles,
                       unsigned long arraySize) : Object::Object(physicalCoords, modelColors,
                                                                 glm::translate(glm::mat4(1.0f), position + direction * 5.0f), triangles, arraySize) {

    // for stopping criterion
    age = 0;
    speed = 30.0;
    dead = false;

    // save direction and position model
    this->direction = direction;
    this->position = position + this->direction  * 5.0f;
}

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

bool Projectile::move(float deltaTime){
    if(dead)
        return false;

    /// ------ UPDATE MODEL -----------------------
    /// == create new matrices ===

    /// --- update scale ----

    /// --- update ROTATION ----
    updateRotation(deltaTime);

    /// ---- update position (== new position) -----
    age += 1;
    position += direction * deltaTime * speed;
    Model = glm::translate( glm::mat4(1.0f), position); // move in position



    /// --- UPDATE ALL COORDINATES -----------
    calculateAllCoords();

    if(age > 500 || speed < 0)
        dead = true;

    return true;
}

void Projectile::updateRotation(float deltaTime) {

    // wee need old quaternion
    glm::quat oldQuat;
    glm::quat deltaQuat;

    glm::quat newQuat = oldQuat * deltaQuat;



    // RotationAngle is in radians
    GLfloat x = RotationAxis.x * sin(RotationAngle / 2);
    GLfloat y = RotationAxis.y * sin(RotationAngle / 2);
    GLfloat z = RotationAxis.z * sin(RotationAngle / 2);
    GLfloat w = cos(RotationAngle / 2);


    // Creates an identity quaternion (no rotation)
    glm::quat MyQuaternion;

    // Direct specification of the 4 components
// You almost never use this directly
    MyQuaternion = quat(w,x,y,z);

// Conversion from Euler angles (in radians) to Quaternion
    glm::vec3 EulerAngles(90, 45, 0);
    MyQuaternion = glm::quat(EulerAngles);

    glm::mat4 RotationMatrix = glm::toMat4(MyQuaternion);


    GLfloat x_rot_speed = 3.0f; // 1otocka == 3s
    GLfloat y_rot_speed = 1.0f; // 1otocka == 1s
    GLfloat z_rot_speed = 5.0f; // 1otocka == 7s

    GLfloat x_angle_delta = 2.0f * 3.14f * (deltaTime / x_rot_speed); // to je ale oproti minule pozici (jakoze 2Pi radianu)

}
