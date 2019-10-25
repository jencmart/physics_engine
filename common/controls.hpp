//
// Created by jencmart on 10/18/19.
//

#ifndef OPENGL_GAME_CONTROLS_HPP
#define OPENGL_GAME_CONTROLS_HPP

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic. ? MAGIC ?
#include <GL/glew.h>

// Include GLFW  - handle window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - opengl mathematics
#include <glm/glm.hpp>

// Easy to use matrix transformations
#include <glm/gtc/matrix_transform.hpp>


#include <unistd.h>


class Controls{
private:
    // position
    glm::vec3 position = glm::vec3( 0, 0, 5 );

    // horizontal angle : toward -Z
    double horizontalAngle = 3.14f;

    // vertical angle : 0, look at the horizon
    double verticalAngle = 0.0f;

    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 70.0f; // 3 units / second
    double mouseSpeed = 0.09f;

    GLFWwindow* window;

    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;

    glm::vec3 direction;

public:

    Controls(GLFWwindow* Window){
        this->window = Window;
    };

    void computeMatricesFromInputs(float deltaTime){

        // Get mouse position
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        // Reset mouse position for next frame
        glfwSetCursorPos(window, 1024.0/2, 768.0/2);

        // Compute new orientation
        horizontalAngle += mouseSpeed * deltaTime * float(1024.0/2 - xpos );
        verticalAngle   += mouseSpeed * deltaTime * float( 768.0/2 - ypos );


        // Direction : Spherical coordinates to Cartesian coordinates conversion
        direction = glm::vec3 (
                cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle)
        );

        // Right vector
        glm::vec3 right = glm::vec3(
                sin(horizontalAngle - 3.14f/2.0f),
                0,
                cos(horizontalAngle - 3.14f/2.0f)
        );

        // Up vector
        glm::vec3 up = glm::cross( right, direction );

        // Move forward
        if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
            position += direction*deltaTime * speed;
        }
        // Move backward
        if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
            position -= direction * deltaTime * speed;
        }
        // Strafe right
        if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
            position += right * deltaTime * speed;
        }
        // Strafe left
        if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
            position -= right * deltaTime * speed;
        }

        float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.


        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 1000.0f);
        // Camera matrix
        ViewMatrix= glm::lookAt(
                position,           // Camera is here
                position+direction, // and looks here : at the same position, plus "direction"
                up                  // Head is up (set to 0,-1,0 to look upside-down)
        );
    }

    glm::vec3  getDirection(){
        return direction;
    }


    glm::vec3 getPosition(){
        return position;
    }

    glm::mat4 getViewMatrix(){
        return ViewMatrix;
    }

    glm::mat4 getProjectionMatrix(){
        return ProjectionMatrix;
    }
};


#endif //OPENGL_GAME_CONTROLS_HPP
