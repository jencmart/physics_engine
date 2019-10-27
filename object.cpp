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

//#include "boundingArea.hpp"
#include "object.hpp"

void Object::stop(){
     dead = true;
}

void Object::changeColor(int colorID){

    colorID += 1;
    for(int  i = 0; i < (int)colorData.size() ; ++i){
        GLfloat one = 1.0f;
        GLfloat  zero = 0.0f;

        if(i%colorID == 0){
            colorData[i] = one;
        } else
            colorData[i] = zero;
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
: currentQuat(), ScaleMatrix(), physicalCoords(physicalCoords), colorData(modelColors), Model(modelMatrix), triangles(triangles), arraySize(arraySize){
    if(triangles)
        cntElementsToDraw = (int)physicalCoords.size()/3;
    else
        cntElementsToDraw = (int)physicalCoords.size()/2;
    octreeNode = nullptr;
    boundingBox = nullptr;
    obb = nullptr;

    this->outOfTree = true;

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