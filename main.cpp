#include <iostream>

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic. ? MAGIC ?
#include <GL/glew.h>

// Include GLFW  - handle window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - opengl mathematics
#include <glm/glm.hpp>

// Easy to use matrix transformations
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>

#include <unistd.h>


#include "common/shader.hpp"
#include "common/controls.hpp"
#include "models/objects.hpp"
#include "octree.hpp"
#include "Projectile.hpp"

#define dim(x) (sizeof(x) / sizeof((x)[0]))

using namespace glm;

// apt install libqt4-opengl
// apt install libglew-dev
// apt install libglfw3-dev
// apt install libglm-dev   # opengl mathematics

// GLSL Support plugin for CLion


GLFWwindow *createSimpleWindow(int width, int height, const std::string& text) {

    glewExperimental = true; // Needed for core profile
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return nullptr;
    }

    // window properties
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); // We want OpenGL 3.3
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

    // window itself
    GLFWwindow* window;
    window = glfwCreateWindow( width, height, text.c_str(), nullptr, nullptr);
    if( window == nullptr ){
        fprintf( stderr, "Failed to open GLFW window. Maybe you have an Intel GPU which are not 3.3 compatible. \n" );
        glfwTerminate();
        return nullptr;
    }

    return window;
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

void mouse_button_callback(GLFWwindow* win, int button, int action, int mods);

class Timer {
private:
        double lastTime;
public:
    Timer(){
        // glfwGetTime is called only once, the first time this function is called
        lastTime = glfwGetTime();
    }

    float getTimeDelta(){
        // Compute time difference between current and last frame
        double currentTime = glfwGetTime();
        auto deltaTime = (float) (currentTime - lastTime);

        // For the next frame, the "last time" will be "now"
        lastTime = currentTime;
        return deltaTime;
    }

    void reset(){
        lastTime = glfwGetTime();
    }
};

#include "models/Sphere.hpp"


class MainLoop{

public:
    GLFWwindow* window = nullptr;
    Controls* controls = nullptr;
    std::vector<Object *> movableObjects;
    Octree * octree;

    int init(){
        // Init GLFW Window
        window = createSimpleWindow(1024, 768, "OpenGL - let's do it");
        if(!window)
            return -1;
        glfwMakeContextCurrent(window);

        // Init GLEW
        glewExperimental=true; // Needed in core profile
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            return -1;
        }
        glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);     // Ensure we can capture the escape key
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

//        glDepthRange(0,100);


        // Enable depth test  -- The Z-Buffer -- fix the problem of overlapping invisible vertices
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        // Dark blue background
        glClearColor(0.4f, 0.4f, 0.7f, 0.0f);


        // Cull triangles which normal is not towards the camera ( do not draw hidden verticies)
        glEnable(GL_CULL_FACE);
        return 1;
    }


    void run(){

        /// -------------------- LOAD SHADERS --------------------------------------------------------------- ///
        GLuint shaderProgramID = LoadShaders("../common/SimpleVertexShader.glsl", "../common/SimpleFragmentShader.glsl" );

        // Get a handle for our "MVP" uniform -- ocividne to bude nakej pseudo pointer na matici 4x4 ktera je ve shaderu
        GLuint shaderMVPMatrixID = glGetUniformLocation(shaderProgramID, "MVP");

        /// -------------------- INIT VERTEX ARRAY --------------------------------------------------------- ///
        GLuint VertexArrayID;       // ID for VAO (vertex array object)
        glGenVertexArrays(1, &VertexArrayID); // create it && save ID // extent_e.g. VertexArrayID = 1 (can be 2, 3 ...)
        glBindVertexArray(VertexArrayID);  //  Make the new array active, creating it if necessary.


        /// -------------------- INIT VERTEX & COLOR BUFFER -------------------------------------------------------- ///
        GLuint vertexBuffer;  // ID for buffer "unsigned int"
        glGenBuffers(1, &vertexBuffer); // create it && save ID (can be 2, 3 ...)
        GLuint colorbuffer;
        glGenBuffers(1, &colorbuffer);


        /// -------------------- INIT CONTROL CLASS -------------------------------------------------------------
        controls = new Controls(window);

        // Now, associate the wrapper to the window
        glfwSetWindowUserPointer(window, this);

        glfwSetMouseButtonCallback(window, mouse_button_callback);


        std::vector<Object*> staticObjects;

        /// ------------------ CREATE STATIC ELEMENTS ----------------------------------------------------------  ///
        // Create STATIC elements
        for(int i = 1 ; i < 0; i++) {

            int x = rand() % 45 + 1;  // [1,45]
            int y = rand() % 45 + 1;  // [1,45]
            int z = rand() % 45 + 1;  // [1,45]

            if(x%2)
                x = -x;
            if(y%2)
                y = -y;
            if(z%2)
                z = -z;

            // M matrix
            glm::mat4 M = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

            // load physical model
            std::vector<GLfloat> loadedModel(myObject::cube, myObject::cube + 36 * 3);

            // now should be good
            std::vector<GLfloat> loadedColors(myObject::cubeColor, myObject::cubeColor + 36 * 3); // 6 bodu na strane , 6 stran , 3 sour

            // create new Object
            auto * object = new Object(loadedModel, loadedColors, M, true, sizeof(myObject::cubeColor));
            staticObjects.emplace_back(object);
        }

        //// ----- BUID THE OCTREE ----- ///
        float min = -100;

        octree = new Octree(glm::vec3(min, min, min), glm::vec3(-min, -min, -min));
        for(auto & object : staticObjects){
            octree->add(object);
        }

        /// --- LOAD PLANE ----
        // load physical model
        std::vector<GLfloat> loadedModel(myObject::plane, myObject::plane + 6 * 3);

        // now should be good
        std::vector<GLfloat> loadedColors(myObject::planeColor, myObject::planeColor + 6 * 3);

        auto * plane = new Object(loadedModel,  loadedColors, glm::mat4(1.0f),true, sizeof(myObject::planeColor));

        octree->add(plane);
        octree->BuildTree();




        Timer timer = Timer();

        ///// -------------------- MAIN LOOP ----------------------------------------------------------------/////
        while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 ) // esc || (X)
        {

            float timeDelta = timer.getTimeDelta();

            //  Update mouse and controls positions
            controls->computeMatricesFromInputs(timeDelta);

            // Add new Objects to the tree
            octree->includeNew();

            std::vector<Object *> updated;

            // Move objects which are movable
            for(auto & movableObject : movableObjects)
                if( movableObject->move(timeDelta))
                    updated.emplace_back(movableObject);

            // update objects which were moved
            octree->updateObjects(updated);

            // Clear the screen (also dept buffer [Z] )
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//

//
//            //// ---- draw static objects
//            plane->draw(vertexBuffer, colorbuffer, controls->getProjectionMatrix(), controls->getViewMatrix(),
//                        shaderProgramID, shaderMVPMatrixID);
//            // planeColor2

            /////------------- DRAW OCTREE ( BB + objects ) ---------------------------------------------/////
            octree->renderTree(vertexBuffer, colorbuffer, controls->getProjectionMatrix(), controls->getViewMatrix(),
                           shaderProgramID, shaderMVPMatrixID, octree->root);


            /// ------------------------- REFRESH WINDOW ------------------------------------------------ ///
            // When we do rendering with OpenGL, the resulting graphics are stored in the BACK buffer.
            // By calling glfwSwapBuffers, we copy the BACK buffer to the FRONT buffer (what we see on the window).
            // This is done to prevent flickering caused by the rendering process being visible.
            glfwSwapBuffers(window);

            // Ask GLFW to get the latest window events and process them. (closing, resizing etc...)
            glfwPollEvents();
        }


        // Cleanup VBO and shader
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &colorbuffer);
        glDeleteProgram(shaderProgramID);
        glDeleteVertexArrays(1, &VertexArrayID);

        // Close OpenGL window and terminate GLFW
        glfwTerminate();
    }
};


void mouse_button_callback(GLFWwindow* win, int button, int action, int mods)
{
    // cast this cursed pointer
    void *data = glfwGetWindowUserPointer(win);
    auto *w = static_cast<MainLoop *>(data);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // load physical model
        std::vector<GLfloat> loadedModel(myObject::cube, myObject::cube + 36 * 3);

        // now should be good
        std::vector<GLfloat> loadedColors(myObject::cubeColor, myObject::cubeColor + 36 * 3);

        Object * newObj = new Projectile(w->controls->getPosition(), w->controls->getDirection(), loadedModel, loadedColors, true,
                                         sizeof(myObject::cube));
        w->movableObjects.emplace_back(newObj);
        w->octree->add(newObj);
    }
}

int main() {

    MainLoop game = MainLoop();
    game.init();
    game.run();

    return 0;
}

#pragma clang diagnostic pop