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

#define OCT_BUILT 1
#define OCT_NOT_BUILT 0



void loadObject(GLuint vertexBuffer, GLuint colorbuffer, const GLfloat* vert , const GLfloat * colors, int size) {
    /// -------------------- LOAD VERTICES & COLORS  OF OBJECT ------------------------------------------------ ///
    // Make the new buffer active, creating it if necessary.
    // Kind of like:
    // if (opengl->buffers[buffer] == null)
    //     opengl->buffers[buffer] = new Buffer()
    // opengl->current_array_buffer = opengl->buffers[buffer]
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // --- switching context --- this cost time from what I get

    // Upload a bunch of data into the active array buffer
    // Kind of like:
    // opengl->current_array_buffer->data = new byte[sizeof(points)]
    // memcpy(opengl->current_array_buffer->data, points, sizeof(points))
    glBufferData(GL_ARRAY_BUFFER, size, vert, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer); // activate the buffer
    glBufferData(GL_ARRAY_BUFFER, size, colors, GL_STATIC_DRAW);
}


void render(GLuint vertexbuffer, GLuint colorbuffer, int fromVertex, int cntElementsToDraw, bool triangles){

    GLfloat lineWidthRange[7];
    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);

    auto type = GL_TRIANGLES;
    if(!triangles)
        type = GL_LINES;

    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
    );

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
    );

    // Draw it to the back buffer !
    glDrawArrays(type, fromVertex, cntElementsToDraw); // 12*3 ==  12 triangles  ; 12*2 == 12 lines

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

class BoundingBox{
public:
    std::vector<glm::vec3> corners;
    std::vector<GLfloat> red;
    std::vector<GLfloat> green;
    glm::vec3 max; // horni radka
    glm::vec3 min; // dolni radka

    long unsigned arraySize = 288;

    std::vector<GLfloat> getLinesList(){
        std::vector<GLfloat> list;
        for(int i = 0 ; i < 3; ++i){ /// A--B--C--D
            // start edge
            list.emplace_back(corners[i][0]);
            list.emplace_back(corners[i][1]);
            list.emplace_back(corners[i][2]);

            // end edge
            list.emplace_back(corners[i+1][0]);
            list.emplace_back(corners[i+1][1]);
            list.emplace_back(corners[i+1][2]);
        }

        /// D--A
        // start edge
        list.emplace_back(corners[0][0]);
        list.emplace_back(corners[0][1]);
        list.emplace_back(corners[0][2]);

        // end edge
        list.emplace_back(corners[3][0]);
        list.emplace_back(corners[3][1]);
        list.emplace_back(corners[3][2]);


        for(int i = 4 ; i < 7; ++i){ /// E--F--G--H
            // start edge
            list.emplace_back(corners[i][0]);
            list.emplace_back(corners[i][1]);
            list.emplace_back(corners[i][2]);

            // end edge
            list.emplace_back(corners[i+1][0]);
            list.emplace_back(corners[i+1][1]);
            list.emplace_back(corners[i+1][2]);
        }

        /// H--E
        // start edge
        list.emplace_back(corners[7][0]);
        list.emplace_back(corners[7][1]);
        list.emplace_back(corners[7][2]);

        // end edge
        list.emplace_back(corners[4][0]);
        list.emplace_back(corners[4][1]);
        list.emplace_back(corners[4][2]);



        for(int i = 0 ; i < 4; ++i){ /// A---E ; B---F ; C---G; D---H  // vertical lines
            // start edge
            list.emplace_back(corners[i][0]);
            list.emplace_back(corners[i][1]);
            list.emplace_back(corners[i][2]);

            // end edge
            list.emplace_back(corners[i+4][0]);
            list.emplace_back(corners[i+4][1]);
            list.emplace_back(corners[i+4][2]);
        }
        return list;

    }

    BoundingBox(glm::vec3 min, glm::vec3 max) : min(min), max(max), green(myObject::cubeOutlineColorGreen, myObject::cubeOutlineColorGreen + 72), red(myObject::cubeOutlineColorRed, myObject::cubeOutlineColorRed + 72) {

        this->corners.emplace_back(glm::vec3(min[0], min[1], min[2])); // A
        this->corners.emplace_back(glm::vec3(max[0], min[1], min[2])); // B
        this->corners.emplace_back(glm::vec3(max[0], max[1], min[2])); // C
        this->corners.emplace_back(glm::vec3(min[0], max[1], min[2])); // D

        this->corners.emplace_back(glm::vec3(min[0], min[1], max[2])); // E
        this->corners.emplace_back(glm::vec3(max[0], min[1], max[2])); // F
        this->corners.emplace_back(glm::vec3(max[0], max[1], max[2])); // G
        this->corners.emplace_back(glm::vec3(min[0], max[1], max[2])); // H
    };

    bool contains(const BoundingBox * x) const {
        return x->min[0] > min[0] && x->min[1] > min[1] && x->min[2] > min[2] &&
               x->max[0] < max[0] &&  x->max[1] < max[1] && x->max[2] < max[2];
    }

     float getWidth() const {
        return max[1] - min[1];
    }

    glm::vec3 getCenter(){
        return (max + min)/2.0f;
    }

    Interval * getInterval(int axis){
        return  new Interval(this->getBoundingBox()->min[axis], this->getBoundingBox()->max[axis]);
    }

    bool intersects(const BoundingBox *other) {
        return (this->getInterval(0)->overlaps(other->getInterval(0)) &&
                this->getInterval(1)->overlaps(other->getInterval(1)) &&
                this->getInterval(2)->overlaps(other->getInterval(2))
        );


    }
};

class Object;

class OctreeNode {

public:
    BoundingBox region;
    OctreeNode * parent;
    std::list<Object*> objects;
    std::vector<OctreeNode*> children; // 8 children

    OctreeNode(glm::vec3 min, glm::vec3 max) : region(min, max), objects(), children(){
        parent = nullptr;
    };
};



class Interval{
    float min;
    float max;
public:
    Interval(float min, float max) : min(min), max(max) {};
    bool overlaps(Interval * other){
        if(this->min < other->min){
            return this->max < other->min;
        }
        return other->max < this->min;
    }
};
class Object{
private:
    BoundingBox * boundingBox;

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

    unsigned long arraySize;

    bool collide(Object * other){
        return this->boundingBox->intersects(other->getBoundingBox());


    }

    virtual bool move(float deltaTime){
        return false;
    }

    void calculateRealCoords(){
        glm::vec3 min;
        glm::vec3 max;

       // if( ! modelSpaceCoordsVector.empty())
        modelSpaceCoordsVector.clear();
        delete this->boundingBox; // it is ok ; we can delete nullptr

        ///  Calculate actual coords - this is shit, right ?
        for(unsigned long i = 0; i < physicalCoords.size() - 2; i = i + 3){



            glm::vec4 pointCoords(physicalCoords[i], physicalCoords[i + 1], physicalCoords[i + 2], 1.0f);
            glm::vec4 modelSpaceCoords = Model * pointCoords;

            modelSpaceCoordsVector.emplace_back(modelSpaceCoords[0]);
            modelSpaceCoordsVector.emplace_back(modelSpaceCoords[1]);
            modelSpaceCoordsVector.emplace_back(modelSpaceCoords[2]);

            if(i == 0){
                min = glm::vec3(modelSpaceCoords[0], modelSpaceCoords[1], modelSpaceCoords[2]);
                max = glm::vec3(modelSpaceCoords[0], modelSpaceCoords[1], modelSpaceCoords[2]);
            }

            /// MIN -- min in each direction
            if( modelSpaceCoords[0] < min[0] )
                min[0] = modelSpaceCoords[0];
            if(modelSpaceCoords[1] < min[1])
                min[1] = modelSpaceCoords[1];
            if(modelSpaceCoords[2] < min[2])
                min[2] = modelSpaceCoords[2];

            /// MAX --- max in each direction
            if( modelSpaceCoords[0] > max[0])
                max[0] = modelSpaceCoords[0];
            if(modelSpaceCoords[1] > max[1])
                max[1] = modelSpaceCoords[1];
            if(modelSpaceCoords[2] > max[2])
                max[2] = modelSpaceCoords[2];
        }

        // point [A] b c d ; [E] f g h


        boundingBox = new BoundingBox(min, max);

    }

    void draw(GLuint vertexBuffer, GLuint colorbuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLuint shaderProgramID, GLuint shaderMVPMatrixID){

        // Load base object
        loadObject(vertexBuffer, colorbuffer, &(this->physicalCoords[0]), &(this->colorData[0]), (int)this->arraySize);


        // ModelViewProjection
        glm::mat4 mvpObj =projectionMatrix * viewMatrix * this->Model;

        // let OpenGL use this shader (we have usually multiple shaders)
        glUseProgram(shaderProgramID);

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        glUniformMatrix4fv(shaderMVPMatrixID, 1, GL_FALSE, &mvpObj[0][0]);

        render(vertexBuffer, colorbuffer, 0, this->cntElementsToDraw, this->triangles);

    }

    Object(std::vector<GLfloat> physicalCoords, std::vector<GLfloat> modelColors, glm::mat4 modelMatrix, bool triangles,
           unsigned long arraySize)
            : physicalCoords(physicalCoords), colorData(modelColors), Model(modelMatrix), triangles(triangles), arraySize(arraySize){
        if(triangles)
            cntElementsToDraw = (int)physicalCoords.size()/3;
        else
            cntElementsToDraw = (int)physicalCoords.size()/2;
        octreeNode = nullptr;
        boundingBox = nullptr;

        this->outOfTree = true;

        calculateRealCoords();

    };

    const BoundingBox * getBoundingBox(){
        return boundingBox;
    }

    void outOfTheTree() {
        this->outOfTree = true;
        octreeNode = nullptr;

    }

    void isInTree() {
        this->outOfTree = false;
    }
};

class Projectile : public Object{

public:
    glm::vec3 direction; // for updating model matrix
    glm::vec3 position; // for updating model matrix
    float speed;         // for updating model matrix

    float age;
    bool dead;

    Projectile(glm::vec3 position,
               glm::vec3 direction,
               std::vector<GLfloat> physicalCoords,
               std::vector<GLfloat> modelColors,
               bool triangles,
               unsigned long arraySize) : Object(physicalCoords, modelColors,
                                        glm::translate(glm::mat4(1.0f), position + direction * 5.0f), triangles, arraySize) {

        // for stopping criterion
        age = 0;
        speed = 30.0;
        dead = false;

        // save direction and position model
        this->direction = direction;
        this->position = position + this->direction  * 5.0f;
    };

    bool move(float deltaTime) override{
        if(dead)
            return false;

        // Compute time difference between current and last frame
        double currentTime = glfwGetTime();
       // auto deltaTime = (float) (currentTime - lastTime);
        age += 1;

        position += direction * deltaTime * speed;
        Model = glm::translate( glm::mat4(1.0f), position); // move in position


        // todo -- update the coordinates && bounding box ?
        calculateRealCoords();

     //   lastTime = currentTime;
        if(age < 500)
            speed = speed - 1/speed;
        else
            dead = true;
        if(speed < 0)
            dead = true;

        return true;
    }

    bool isDead(){
        return dead;
    }
};


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

        std::vector<GLfloat> boundingBoxLines = node->region.getLinesList(); // to zni OK

        // Load base object
        if(node->objects.empty())
            loadObject(vertexBuffer, colorbuffer, &boundingBoxLines[0], &(node->region.green[0]) , (int)node->region.arraySize); // to zni OK
        else
            loadObject(vertexBuffer, colorbuffer, &boundingBoxLines[0], &(node->region.red[0]), (int)node->region.arraySize); // to zni OK


        // MVP
        glm::mat4 mvp = projectionMatrix * viewMatrix; // v modelu uz to jakoby mame

        // load shader && bind MVP to it
        glUseProgram(shaderProgramID);
        glUniformMatrix4fv(shaderMVPMatrixID, 1, GL_FALSE, &mvp[0][0]);

        // draw
        render(vertexBuffer, colorbuffer, 0, 12*2, false);

        /// render objects inside the node
        for(auto & object : node->objects){

            object->draw(vertexBuffer, colorbuffer, projectionMatrix, viewMatrix, shaderProgramID,
                         shaderMVPMatrixID);

        }


        for(auto & child : node->children)
            renderTree( vertexBuffer,  colorbuffer, projectionMatrix,viewMatrix,  shaderProgramID , shaderMVPMatrixID,child);
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
//            std::cout << root->objects.size() << std::endl;
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
            object->outOfTheTree(); // todo - do something else when out of the trees
            return;
        }

        // bubble up -> will be inserted
        bubbleUp(node->parent, object);
    }

    void getListOfCollisions(Object *object, OctreeNode * node, std::vector<Object*> & listCollision) {

        // check if it make sense to check for collisions - this never happens right now
        if( ! node->region.contains(object->getBoundingBox()))
            return;

        // check for collisions in current node
        for(auto & obj : node->objects){
            if(object->collide(obj))
                listCollision.emplace_back(obj);
        }

        // check where to go
        for(auto & children : node->children)
            if(children->region.contains(object->getBoundingBox()))
                getListOfCollisions(object, children, listCollision);
    }

    void updateObjects(std::vector<Object*> updated) {
        for(auto & object : updated){
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
//            std::cout << node->objects.size() << "  ";

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

//            std::cout << node->objects.size() << std::endl;

            // we can only go down
            if(node->region.contains(object->getBoundingBox())) {
                this->includeRecursive(object, node);
            }

            else {
               this->bubbleUp(node, object);
             //   this->includeRecursive(object, root);
           //     std::cout << "we are about to get fucked " << std::endl;
            }

            /// ---- CHECK FOR COLIDING OBJECTS -------
            std::vector<Object*> collidedObj;
            this->getListOfCollisions(object, root, collidedObj);
            if(!collidedObj.empty())
                exit(22);
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

        glm::vec3 cornerA = node->region.corners[0];


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
