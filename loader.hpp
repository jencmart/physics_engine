//
// Created by jencmart on 10/27/19.
//

#ifndef OPENGL_GAME_LOADER_HPP
#define OPENGL_GAME_LOADER_HPP

// Include GLEW. Always include it before gl.h and glfw3.h, since it's a bit magic. ? MAGIC ?
#include <GL/glew.h>

// Include GLFW  - handle window and keyboard
#include <GLFW/glfw3.h>

// Include GLM - opengl mathematics
#include <glm/glm.hpp>

// Easy to use matrix transformations
#include <glm/gtc/matrix_transform.hpp>

class Loader{
public:

    static void loadObject(GLuint vertexBuffer, GLuint colorbuffer, const GLfloat* vert , const GLfloat * colors, int size) {
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

    static void render(GLuint vertexbuffer, GLuint colorbuffer, int fromVertex, int cntElementsToDraw, bool triangles){

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

};



#endif //OPENGL_GAME_LOADER_HPP
