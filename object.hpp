//
// Created by jencmart on 10/27/19.
//

#ifndef OPENGL_GAME_OBJECT_HPP
#define OPENGL_GAME_OBJECT_HPP

#include "octreeNode.hpp"


class Object {
protected:
    AABB * boundingBox;
    OBB * obb;
    glm::vec3 force;

public:
    std::vector<GLfloat> physicalCoords;
    std::vector<GLfloat> modelSpaceCoordsVector;
    std::vector<GLfloat> colorData;
    glm::mat4 Model;
    bool triangles;
    int cntElementsToDraw;
    OctreeNode * octreeNode;
    bool outOfTree;
    bool dead;
    unsigned long arraySize;

    /// ------- FUNCTIONS -------------------------
    void stop();
    void changeColor(int colorID);
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
};
#endif //OPENGL_GAME_OBJECT_HPP
