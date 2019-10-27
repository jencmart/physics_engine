//
// Created by jencmart on 10/21/19.
//

#ifndef OPENGL_GAME_OCTREE_HPP
#define OPENGL_GAME_OCTREE_HPP


// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic. ? MAGIC ?
#include <GL/glew.h>

// Include GLFW  - handle window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - opengl mathematics
#include <glm/glm.hpp>

// Easy to use matrix transformations
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include <vector>
#include <queue>
#include <list>
#include <set>

#include "boundingArea.hpp"
#include "object.hpp"

#define OCT_BUILT 1
#define OCT_NOT_BUILT 0



/**
*****************************************
****  OCTREE IMPLEMETATION **************
*****************************************
*/

class Octree {
private:
    glm::vec3 min;
    glm::vec3 max;
    int state;
    unsigned minCntObjectsInNode = 1;
    std::list<Object*> queue;

    // std::vector<OctreeNode*> nodesArray;

public:
    OctreeNode * root;

    void includeNew(){
        if(queue.empty())
            return;

        for (auto i = queue.begin(); i != queue.end();) {
            Object * object = *i;
            object->isInTree();
            this->includeRecursive(object, root);
            i = queue.erase(i);
        }
    }
//
//    std::vector<OctreeNode*> getNodes(){
//        return nodesArray;
//    }

    void renderTree(GLuint vertexBuffer, GLuint colorbuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLuint shaderProgramID, GLuint shaderMVPMatrixID, OctreeNode * node ) {

        if(! node)
            return;

   //     if(node == root)
//           node->region.renderBoundingArea(vertexBuffer, colorbuffer, projectionMatrix, viewMatrix, shaderProgramID,shaderMVPMatrixID);

        /// render objects inside the node
        for(auto & object : node->objects){

            object->draw(vertexBuffer, colorbuffer, projectionMatrix, viewMatrix, shaderProgramID,
                         shaderMVPMatrixID);

          //  object->getBoundingBox()->renderBoundingArea(vertexBuffer, colorbuffer, projectionMatrix, viewMatrix, shaderProgramID, shaderMVPMatrixID);

            object->getOBB()->renderBoundingArea(vertexBuffer, colorbuffer, projectionMatrix, viewMatrix, object->Model, shaderProgramID,
                                                   shaderMVPMatrixID);

        }


        for(auto & child : node->children)
            renderTree(vertexBuffer,  colorbuffer, projectionMatrix,viewMatrix,  shaderProgramID , shaderMVPMatrixID,child);
    }

    Octree(glm::vec3 min, glm::vec3 max): min(min), max(max){
        state = OCT_NOT_BUILT;
        root = nullptr;
    };


    void add(Object * object) {
        queue.emplace_back(object);
    }

    // can be called only once
    void BuildTree(){
        if(root == nullptr){
            root = new OctreeNode(min, max);
            root->objects = std::move(queue);
            this->buildRecursive(root);
            state = OCT_BUILT;
            return;
        }
    }


    OctreeNode * canEmplaceToChild(OctreeNode * node, Object * object){
        for(auto & child: node->children) {
            // if object is inside bounding box of the child node, we move it there and continue with new element
            if(child->region.contains(object->getBoundingBox()) ){
                return child;
            }
        }
        return nullptr;
    }


    void bubbleUp(OctreeNode * node, Object * object){

        //   stopping criterion
        if(node->region.contains(object->getBoundingBox())) {
            this->includeRecursive(object, node); // == bubble down
            return;
        }

        // If I am empty && all my childrens are empty ( I have them, thus I am not leaf ) ->  delete the childrens
        if(node->objects.empty() && ! node->children.empty()) {

            // check the emptiness
            bool allChildEmpty = true;
            for(auto & child : node->children){
                if(child->children.empty() && child->objects.empty())
                    continue;
                else
                    allChildEmpty = false;
            }

            // delete children
            if(allChildEmpty)
                node->children.clear();
        }


        // try to push the object up
        if(node->parent == nullptr){
            object->direction = -1.0f * object->direction;
            node->objects.emplace_back(object);
            object->octreeNode = node; // safe fallback
            buildRecursive(node);
         //   object->outOfTheTree(); // todo - do something else when out of the trees -- right now we just reverse direction -- todo --- do the colision resolution also !!!!
            return;
        }

        // bubble up -> will be inserted
        bubbleUp(node->parent, object);
    }

    void getListOfCollisions(Object *object, OctreeNode * node, std::vector<Object*> & listCollision) {

        // check if it make sense to check for collisions - this never happens right now
        if( !node->region.intersects(object->getBoundingBox()))
            return;

        // check for collisions in current node
        for(auto & obj : node->objects){
            if(object->getOBB()->intersects(obj->getOBB())) // todo big changeeero
                listCollision.emplace_back(obj);
        }

        // check where to go
        for(auto & children : node->children)
            if(children->region.intersects(object->getBoundingBox()))
                getListOfCollisions(object, children, listCollision);
    }

    void updateObjects(std::vector<Object*> updated) {


        for(auto & object : updated){
            /// clear this because later we will deal with new collisions
            object->allreadyCollided.clear();

            OctreeNode * node = object->octreeNode;

            if(node == nullptr){


                if(root->region.contains(object->getBoundingBox()))
                {
                    this->includeRecursive(object, root);
                }
                else{
                    continue; // todo - that means that we do not explicitly informs that object is out of the tree and we are only waiting if it will return sometime ... AND WE DO NOT DELETE THE OBJECT !!!!!!! that is bullshit
                }
            }

            for (auto i = node->objects.begin(); i != node->objects.end();) {
                if ( *i == object){
                     node->objects.erase(i);
                    break;
                }
                else{
                    ++i;
                }
            }

            object->octreeNode = nullptr;

            // we can only go down
            if(node->region.contains(object->getBoundingBox())) {
                this->includeRecursive(object, node);
            }

            else {
               this->bubbleUp(node, object);
             //   this->includeRecursive(object, root);
            }
        }

        /// ---- CHECK FOR COLIDING OBJECTS -------
        for(auto & object : updated){
            std::vector<Object*> collidedObj;
            this->getListOfCollisions(object, root, collidedObj);
            if(!collidedObj.empty())
            {
                for(auto & obj : collidedObj){
                    if( ! obj->fixed){
                        obj->changeColor(obj->colorR/10.0f, obj->colorG/10.0f, obj->colorB/10.0f);
                        obj->colorR = (obj->colorR+(rand() % 10 + 1))%10;
                        obj->colorG = (obj->colorG+(rand() % 10 + 1))%10;
                        obj->colorB = (obj->colorB+(rand() % 10 + 1))%10;
                    }

                    object->collisionResolution(obj);
                }
                object->changeColor(object->colorR/10.0f, object->colorG/10.0f, object->colorB/10.0f);
                object->colorR = (object->colorR+(rand() % 10 + 1))%10;
                object->colorG = (object->colorG+(rand() % 10 + 1))%10;
                object->colorB = (object->colorB+(rand() % 10 + 1))%10;
            }
        }
    }

    void includeRecursive(Object* toInsert, OctreeNode * node){

        // if I am leaf --> I need to create sub nodes
        if(node->children.empty()){
            node->objects.emplace_back(toInsert);
            toInsert->octreeNode = node; // safe fallback
            buildRecursive(node);
            return;
        }

        // if I am not leaf ->
        OctreeNode * child = this->canEmplaceToChild(node, toInsert);


        if (child) { // mam nake dite tak to soupnu do nej
         includeRecursive(toInsert, child);
            return;
        }

        // insert here
        node->objects.emplace_back(toInsert);
        toInsert->octreeNode = node;
    }

    void buildRecursive(OctreeNode * node) { // todo mozna naky problem s object -> node ; mam pocit ze nekde se to nebude dit a zustane tam null

        // we quit splitting if threshold is reached
        if (node->objects.size() <= minCntObjectsInNode)
            return;

        // calculate dimensions of current node
        float width = node->region.getWidth();

        // if smaller nodes are too small, we return // todo
        if(width < 1)
            return;

        // we calculate dimensions for children
        float halfWidth = width / 2;
        glm::vec3 center = node->region.getCenter();

        glm::vec3 moveRight(halfWidth, 0, 0);
        glm::vec3 moveBack(0, halfWidth, 0);
        glm::vec3 moveUp(0, 0, halfWidth);

        glm::vec3 cornerA = node->region.boundingBoxCornersForPrint[0];


        // we create 8 children with correct bounding boxes
        node->children.push_back(new OctreeNode(cornerA,                    center));  // OK
        node->children.push_back(new OctreeNode(cornerA+moveRight,          center+moveRight));
        node->children.push_back(new OctreeNode(cornerA+moveRight+moveBack, center+moveRight+moveBack));
        node->children.push_back(new OctreeNode(cornerA+moveBack,           center+moveBack));

        node->children.push_back(new OctreeNode(cornerA+moveUp,                    center+moveUp));   /////// ta co je vlevo dole
        node->children.push_back(new OctreeNode(cornerA+moveUp+moveRight,          center+moveUp+moveRight));
        node->children.push_back(new OctreeNode(cornerA+moveUp+moveRight+moveBack, center+moveUp+moveRight+moveBack));
        node->children.push_back(new OctreeNode(cornerA+moveUp+moveBack,           center+moveUp+moveBack));


        // now we need to calculate where each object corresponds
        for (auto i = node->objects.begin(); i != node->objects.end();) {

            OctreeNode * child = this->canEmplaceToChild(node, *i);

            if (child){
                // vloz ho do daneho ditete
                child->objects.emplace_back(*i);
                (*i)->octreeNode = child;

                // odstran ho odsus
                i = node->objects.erase(i);
            }
            else{
                (*i)->octreeNode = node; // this is our stop
                ++i;
            }
        }


        // we call build on each children
        for (auto &child: node->children) {
            // finally set the parent node on child
            child->parent = node;

            // and build subtree
            buildRecursive(child);
        }
    }
};

#endif //OPENGL_GAME_OCTREE_HPP
