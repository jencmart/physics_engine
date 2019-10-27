//
// Created by jencmart on 10/27/19.
//

#ifndef OPENGL_GAME_OCTREENODE_HPP
#define OPENGL_GAME_OCTREENODE_HPP


#include "boundingArea.hpp"

#include <list>

class Object;

class OctreeNode {
public:
    AABB region;
    OctreeNode * parent;
    std::list<Object*> objects;
    std::vector<OctreeNode*> children; // 8 children

    OctreeNode(glm::vec3 min, glm::vec3 max) : region(min, max), objects(), children(){
        parent = nullptr;
    };
};

#endif //OPENGL_GAME_OCTREENODE_HPP
