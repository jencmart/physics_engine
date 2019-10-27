//
// Created by jencmart on 10/27/19.
//

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic. ? MAGIC ?
#include <GL/glew.h>

// Include GLFW  - handle window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - opengl mathematics
#include <glm/glm.hpp>

// Easy to use matrix transformations
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <list>
#include <zconf.h>

//#include "boundingArea.hpp"
#include "object.hpp"

void Object::stop(){
     dead = true;
}

void Object::changeColor(GLfloat r, GLfloat g, GLfloat b){

    for(int  i = 0; i < (int)colorData.size() - 2; i += 3){

        colorData[i] = r;
        colorData[i+1] = g;
        colorData[i+1] = b;
    }
}

bool Object::move(float deltaTime){
    return false;
}

void Object::updateOBB(){
    if(obb == nullptr){
        obb = new OBB(physicalCoords);
        return;
    }

    /// -- TRANSLATION ( just centre ) todo - what happens when centre point is not sme as the orign of the model ?? maybe its good to use model matrix I dunno
    glm::vec4 pointCoords(obb->getObjectModel()->position_u[0], obb->getObjectModel()->position_u[1], obb->getObjectModel()->position_u[2], 1.0f);
    glm::vec4 modelSpaceCoords = Model * pointCoords; // * initial point cords // todo -- maybe would make sense to apply only translate matrix

    obb->center_u[0] = modelSpaceCoords[0] ;//- obb->center_u[0];
    obb->center_u[1] = modelSpaceCoords[1] ;//- obb->center_u[1];
    obb->center_u[2] = modelSpaceCoords[2] ;//- obb->center_u[2];

    /// -- ROTATION ( just rotation matrix I guess)
    // vezmu puvodni vektory ( teda ty co byly vypocitane na zactku)
    // a na ne aplikuji rotaci
    glm::vec4 R(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 U(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 F(1.0f, 1.0f, 1.0f, 1.0f);
    // fill it -- todo - i hope i am doing it in the right direction
    R.x = obb->getObjectModel()->rotation_r[0][0]; U.x = obb->getObjectModel()->rotation_r[0][1]; F.x = obb->getObjectModel()->rotation_r[0][2]; // r1 u1 f1
    R.y = obb->getObjectModel()->rotation_r[1][0]; U.y = obb->getObjectModel()->rotation_r[1][1]; F.y = obb->getObjectModel()->rotation_r[1][2]; // r2 u2 f2
    R.z = obb->getObjectModel()->rotation_r[2][0]; U.z = obb->getObjectModel()->rotation_r[2][1]; F.z = obb->getObjectModel()->rotation_r[2][2]; // r3 u3 f3

    R = RotationMatrix * R;
    U = RotationMatrix * U;
    F = RotationMatrix * F;

    /// AND NOW Update the OBB ( DO NOT WRITE IT TO THE OBJECT MODEL :-D )
    // ROTATION MATRIX
    obb->rotation_r[0][0]=R.x; obb->rotation_r[0][1]=U.x; obb->rotation_r[0][2]=F.x; // r1 u1 f1
    obb->rotation_r[1][0]=R.y; obb->rotation_r[1][1]=U.y; obb->rotation_r[1][2]=F.y; // r2 u2 f2
    obb->rotation_r[2][0]=R.z; obb->rotation_r[2][1]=U.z; obb->rotation_r[2][2]=F.z; // r3 u3 f3
}

void Object::updateBoundingBox() {
    delete this->boundingBox;

    glm::vec3 min;
    glm::vec3 max;

    for(int i = 0 ; (int) i < modelSpaceCoordsVector.size() - 2 ; i+=3){
        if(i == 0){
            min = glm::vec3(modelSpaceCoordsVector[0], modelSpaceCoordsVector[1], modelSpaceCoordsVector[2]);
            max = glm::vec3(modelSpaceCoordsVector[0], modelSpaceCoordsVector[1], modelSpaceCoordsVector[2]);
        }

        /// MIN -- min in each direction
        for(int k = i ; k < i+3 ;k++){
            if( modelSpaceCoordsVector[k] < min[k-i] )
                min[k-i] = modelSpaceCoordsVector[k];
        }

        /// MAX --- max in each direction
        for(int k = i ; k < i+3 ;k++){
            if( modelSpaceCoordsVector[k] > max[k-i])
                max[k-i] = modelSpaceCoordsVector[k];
        }
    }

    boundingBox = new AABB(min, max);
}

void Object::calculateAllCoords(){

    ///  ---- COORDS OF OBJECT ------------------
    modelSpaceCoordsVector.clear();
    for(unsigned long i = 0; i < physicalCoords.size() - 2; i = i + 3){
        glm::vec4 pointCoords(physicalCoords[i], physicalCoords[i + 1], physicalCoords[i + 2], 1.0f);
        glm::vec4 modelSpaceCoords = Model * pointCoords;
        modelSpaceCoordsVector.emplace_back(modelSpaceCoords[0]);
        modelSpaceCoordsVector.emplace_back(modelSpaceCoords[1]);
        modelSpaceCoordsVector.emplace_back(modelSpaceCoords[2]);
    }

    ///  ---- COORDS OF AABB ------------------
    updateBoundingBox();

    /// ---- COORDS OF OBB -------------------
    updateOBB();
}

void Object::draw(GLuint vertexBuffer, GLuint colorbuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLuint shaderProgramID, GLuint shaderMVPMatrixID){

    // Load base object
    Loader::loadObject(vertexBuffer, colorbuffer, &(this->physicalCoords[0]), &(this->colorData[0]), (int)this->arraySize);


    // ModelViewProjection
    glm::mat4 mvpObj =projectionMatrix * viewMatrix * this->Model;

    // let OpenGL use this shader (we have usually multiple shaders)
    glUseProgram(shaderProgramID);

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    glUniformMatrix4fv(shaderMVPMatrixID, 1, GL_FALSE, &mvpObj[0][0]);

    Loader::render(vertexBuffer, colorbuffer, 0, this->cntElementsToDraw, this->triangles);

}

Object::Object(std::vector<GLfloat> physicalCoords, std::vector<GLfloat> modelColors, glm::mat4 modelMatrix, bool triangles,
unsigned long arraySize)
: currentQuat(), ScaleMatrix(), allreadyCollided(), physicalCoords(physicalCoords), colorData(modelColors), Model(modelMatrix), triangles(triangles), arraySize(arraySize){
    if(triangles)
        cntElementsToDraw = (int)physicalCoords.size()/3;
    else
        cntElementsToDraw = (int)physicalCoords.size()/2;
    octreeNode = nullptr;
    boundingBox = nullptr;
    obb = nullptr;
    dead = true;
    fixed = true;
    this->outOfTree = true;
    colorR = rand() % 10 + 1;  /* generate secret number between 1 and 10: */
    colorG = rand() % 10 + 1;  /* generate secret number between 1 and 10: */
    colorB = rand() % 10 + 1;  /* generate secret number between 1 and 10: */

    calculateAllCoords();

};

const AABB * Object::getBoundingBox(){
    return boundingBox;
}

OBB * Object::getOBB(){
    return obb;
}

void Object::outOfTheTree() {
    this->outOfTree = true;
    octreeNode = nullptr;

}

void Object::isInTree() {
    this->outOfTree = false;
}

void Object::collisionResolution(Object *secondObject) {
    // me with one or more others

    if(this->fixed) // treat it from the secondObject side
        return;


    // what about solution to the "I treat him, he treats me" ????
    // In one collision pass we only treat two objects one time
    // that means ==> have here list of objects with which you already collided
    // if it is there, skip it ( because you have allready been treated)

    // I treat myself and you ; I do not want to you treat me  (and yourself)
    for( auto & obj : secondObject->allreadyCollided){ // i am at his list, means it allready have been done
        if(obj == this)
            return;
    }
    // otherwise add it to my list
    this->allreadyCollided.emplace_back(secondObject);

    /// ---- 1) work out the direction of the collision. e.g. for a circle/ball hitting a line/surface without friction it will be the perpendicular/normal direction.

    if(secondObject->fixed || secondObject->dead){
        /// todo - dirty fix
        // find the norm
        glm::vec3 norm = glm::normalize( glm::vec3(0,1,0));

        // use this equation
        glm::vec3 reflection(direction -  2*glm::dot(direction,norm)*norm);
        this->direction = glm::normalize(reflection);
    }

    else{

        // find the norm

        // A --> B vector
        glm::vec3 vecX = secondObject->getOBB()->center_u - this->getOBB()->center_u; // A->B vector
        vecX = glm::normalize(vecX);

        // SPHERE A (this)
        GLfloat x1 = glm::dot(vecX, direction * speed);
        glm::vec3 vec_v1_x = vecX * x1; // one direction
        glm::vec3 vec_v1_y = (direction * speed) - vec_v1_x; // second direction

        // SPHERE B (other)
        vecX = -1.0f * vecX;
        GLfloat x2 = glm::dot(vecX, secondObject->direction * secondObject->speed);
        glm::vec3 vec_v2_x = vecX*x2; // one direction
        glm::vec3 vec_v2_y = (secondObject->direction * secondObject->speed) - vec_v2_x; // second direction

        GLfloat m1, m2; // masses
        m1 = 1.0f;
        m2 = 1.0f;

        glm::vec3 thisVelocity = vec_v1_x * (m1-m2) / (m1+m2) + vec_v2_x*(2*m2)/(m1+m2) + vec_v1_y;
        glm::vec3 otherVelocity = vec_v1_x*(2*m1)/(m1+m2) + vec_v2_x*(m2-m1)/(m1+m2) + vec_v2_y;

        this->speed = (float)sqrt(glm::dot(thisVelocity, thisVelocity));
        this->direction = glm::normalize(thisVelocity);

        secondObject->speed = (float)sqrt(glm::dot(otherVelocity, otherVelocity));
        secondObject->direction = glm::normalize(otherVelocity);
    }

    /// ---- 2) work out the impact speed, the speed in the direction of the collision, i.e. the speed towards the line/surface.

    /// ---- 3) work out the impulse. If there''s no rotation it will be simply between 1 and 2 times the impact speed, with 1 being no bounce and 2 being maximum bounce. It is in the direction as the normal/perpendicular, away from the edge/surface.

    ///  ---- 4) Add it to the object velocity to get the velocity after the collision.

}