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
    fixed = false;

    // save direction and position model
    this->direction = glm::normalize(direction);
    this->position = position + this->direction  * 5.0f;
}

bool Projectile::move(float deltaTime){
    if(dead)
        return false;
    else
        age += 1;

    /// --- update SCALE ---- todo (not needed so far)
    updateScale(deltaTime);

    /// --- update ROTATION ---- (RotationMatrix && CurrentQuat) !!
    updateRotation(deltaTime);

    /// ---- update TRANSLATION -----
    updateTranslation(deltaTime);

    Model = TranslationMatrix * RotationMatrix ;//* ScaleMatrix;

    if( speed < 0) // age > 500 ||
        dead = true;

    /// --- UPDATE ALL COORDINATES -----------
    calculateAllCoords();

    return true;
}

void Projectile::updateRotation(float deltaTime) { // usually 0.016 sec


    /// --- WE HAVE SOME SPEED
    GLfloat x_rot_speed = 3.0f; // 1otocka == 3s
    GLfloat y_rot_speed = 1.0f; // 1otocka == 1s
    GLfloat z_rot_speed = 5.0f; // 1otocka == 7s

  //  updateTranslation*= 10;

    /// --- WE CALCULATE THE ANGLES
    GLfloat x_delta = 360.0f * (deltaTime / x_rot_speed); // to je ale oproti minule pozici (jakoze 2Pi radianu)
    GLfloat y_delta = 360.0f * (deltaTime / y_rot_speed); // to je ale oproti minule pozici (jakoze 2Pi radianu)
    GLfloat z_delta = 360.0f * (deltaTime / z_rot_speed); // to je ale oproti minule pozici (jakoze 2Pi radianu)


    //-- rotating around centre ---
    // if we want different origin
    // rotated_point = origin + (orientation_quaternion * (point-origin));

    /// --- WE CREATE THE ROTATION
    // Conversion from Euler angles (in radians) to Quaternion
    glm::vec3 EulerAngles(glm::radians(x_delta), glm::radians(y_delta), glm::radians(z_delta));
    glm::quat  deltaQuat = glm::quat(EulerAngles);

    /// -- AND MAKE FINAL QUATERNION

    if(age == 1)
        this->currentQuat = deltaQuat;
    else
        this->currentQuat = this->currentQuat * deltaQuat;

    /// -- AND ROTATION MATRIX (does not need to be initialized
    this->RotationMatrix = glm::toMat4(this->currentQuat);

//    std::cout << "quat: " << currentQuat.x  << " " << currentQuat.y << " " << currentQuat.z << std::endl;


//    std::cout << std::endl;
//    for(int i = 0 ; i < 4 ; i++){
//        for(int j=0 ; j < 4 ; j++){
//            std::cout << RotationMatrix[i][j] << " " ;
//        }
//        std::cout << std::endl;
//    }
//    std::cout << std::endl;

}

void Projectile::updateScale(float deltaTime) {
        // todo - so far we does not need it
}

void Projectile::updateTranslation(float deltaTime) {
    position += direction * deltaTime * speed;
    TranslationMatrix = glm::translate( glm::mat4(1.0f), position); // move in position (== move the centre of the object...)
}
