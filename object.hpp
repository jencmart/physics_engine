//
// Created by jencmart on 10/27/19.
//

#ifndef OPENGL_GAME_OBJECT_HPP
#define OPENGL_GAME_OBJECT_HPP

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "octreeNode.hpp"


class Object {
protected:
    AABB * boundingBox;
    OBB * obb;
    glm::vec3 force;

public:
    // for projectile
    glm::vec3 direction; // for updating model matrix
    glm::vec3 position; // for updating model matrix
    float speed;         // for updating model matrix
    float age;
    bool fixed;
    int colorR;
    int colorG;
    int colorB;


    // for collision resolution
    std::vector<Object *> allreadyCollided;

    std::vector<GLfloat> physicalCoords;
    std::vector<GLfloat> modelSpaceCoordsVector;
    std::vector<GLfloat> colorData;

    glm::mat4 ScaleMatrix;
    glm::quat currentQuat;
    glm::mat4 RotationMatrix; // todo -- no init
    glm::mat4 TranslationMatrix; // todo -- no init
    glm::mat4 Model;


    bool triangles;
    int cntElementsToDraw;
    OctreeNode * octreeNode;
    bool outOfTree;
    bool dead;
    unsigned long arraySize;

    /// ------- FUNCTIONS -------------------------
    void stop();
    void changeColor(GLfloat r, GLfloat g, GLfloat b);
    virtual bool move(float deltaTime);
    void updateOBB();
    void updateBoundingBox() ;
    void calculateAllCoords();
    void draw(GLuint vertexBuffer, GLuint colorbuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLuint shaderProgramID, GLuint shaderMVPMatrixID);
    Object(std::vector<GLfloat> physicalCoords, std::vector<GLfloat> modelColors, glm::mat4 modelMatrix, bool triangles, unsigned long arraySize);
    const AABB * getBoundingBox();
    OBB * getOBB();
    void outOfTheTree();
    void isInTree();
    void collisionResolution(Object * secondObject);

};
#endif //OPENGL_GAME_OBJECT_HPP
