//
// Created by jencmart on 10/26/19.
//

#ifndef OPENGL_GAME_BOUNDINGAREA_HPP
#define OPENGL_GAME_BOUNDINGAREA_HPP

#include <iostream>

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

#include "loader.hpp"
#include "models/objects.hpp"
#include "lib/eigen/Eigen/LU"
#include "lib/eigen/Eigen/Eigenvalues"




class AABB{
public:
    std::vector<glm::vec3> boundingBoxCornersForPrint;
    std::vector<GLfloat> red;
    std::vector<GLfloat> green;
    glm::vec3 max; // horni radka
    glm::vec3 min; // dolni radka

    long unsigned arraySize = 288;

    void renderBoundingArea(GLuint vertexBuffer, GLuint colorbuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, GLuint shaderProgramID, GLuint shaderMVPMatrixID) const{

        std::vector<GLfloat> boundingBoxLines = this->getLinesList(); // to zni OK

       // bool renderBB = false;

     //   if(renderBB){
            Loader::loadObject(vertexBuffer, colorbuffer, &boundingBoxLines[0], &(red[0]), (int)arraySize); // to zni OK
    //    }

        // MVP
        glm::mat4 mvp = projectionMatrix * viewMatrix; // v modelu uz to jakoby mame

        // load shader && bind MVP to it
        glUseProgram(shaderProgramID);
        glUniformMatrix4fv(shaderMVPMatrixID, 1, GL_FALSE, &mvp[0][0]);

        // draw
        Loader::render(vertexBuffer, colorbuffer, 0, 12*2, false);
    }


    std::vector<GLfloat> getLinesList() const{
        std::vector<GLfloat> list;
        for(int i = 0 ; i < 3; ++i){ /// A--B--C--D
            // start edge
            list.emplace_back(boundingBoxCornersForPrint[i][0]);
            list.emplace_back(boundingBoxCornersForPrint[i][1]);
            list.emplace_back(boundingBoxCornersForPrint[i][2]);

            // end edge
            list.emplace_back(boundingBoxCornersForPrint[i + 1][0]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][1]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][2]);
        }

        /// D--A
        // start edge
        list.emplace_back(boundingBoxCornersForPrint[0][0]);
        list.emplace_back(boundingBoxCornersForPrint[0][1]);
        list.emplace_back(boundingBoxCornersForPrint[0][2]);

        // end edge
        list.emplace_back(boundingBoxCornersForPrint[3][0]);
        list.emplace_back(boundingBoxCornersForPrint[3][1]);
        list.emplace_back(boundingBoxCornersForPrint[3][2]);


        for(int i = 4 ; i < 7; ++i){ /// E--F--G--H
            // start edge
            list.emplace_back(boundingBoxCornersForPrint[i][0]);
            list.emplace_back(boundingBoxCornersForPrint[i][1]);
            list.emplace_back(boundingBoxCornersForPrint[i][2]);

            // end edge
            list.emplace_back(boundingBoxCornersForPrint[i + 1][0]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][1]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][2]);
        }

        /// H--E
        // start edge
        list.emplace_back(boundingBoxCornersForPrint[7][0]);
        list.emplace_back(boundingBoxCornersForPrint[7][1]);
        list.emplace_back(boundingBoxCornersForPrint[7][2]);

        // end edge
        list.emplace_back(boundingBoxCornersForPrint[4][0]);
        list.emplace_back(boundingBoxCornersForPrint[4][1]);
        list.emplace_back(boundingBoxCornersForPrint[4][2]);

        for(int i = 0 ; i < 4; ++i){ /// A---E ; B---F ; C---G; D---H  // vertical lines
            // start edge
            list.emplace_back(boundingBoxCornersForPrint[i][0]);
            list.emplace_back(boundingBoxCornersForPrint[i][1]);
            list.emplace_back(boundingBoxCornersForPrint[i][2]);

            // end edge
            list.emplace_back(boundingBoxCornersForPrint[i + 4][0]);
            list.emplace_back(boundingBoxCornersForPrint[i + 4][1]);
            list.emplace_back(boundingBoxCornersForPrint[i + 4][2]);
        }
        return list;

    }

    AABB(glm::vec3 min, glm::vec3 max) : min(min), max(max), green(myObject::cubeOutlineColorGreen, myObject::cubeOutlineColorGreen + 72), red(myObject::cubeOutlineColorRed, myObject::cubeOutlineColorRed + 72) {

        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(min[0], min[1], min[2])); // A
        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(max[0], min[1], min[2])); // B
        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(max[0], max[1], min[2])); // C
        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(min[0], max[1], min[2])); // D

        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(min[0], min[1], max[2])); // E
        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(max[0], min[1], max[2])); // F
        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(max[0], max[1], max[2])); // G
        this->boundingBoxCornersForPrint.emplace_back(glm::vec3(min[0], max[1], max[2])); // H
    };

    bool contains(const AABB * x) const {
        return x->min[0] >= min[0] &&  x->min[1] >= min[1] && x->min[2] >= min[2] &&
               x->max[0] <= max[0] &&  x->max[1] <= max[1] && x->max[2] <= max[2];
    }

    float getWidth() const {
        return max[1] - min[1];
    }

    glm::vec3 getCenter(){
        return (max + min)/2.0f;
    }

    // this is solution only for the AABB
    bool intersects(const AABB *other) const {

        if (this == other)
            return false;

        const AABB * a = this;
        const AABB * b = other;
        // Exit with no intersection if separated along an axis
        if (a->max[0] < b->min[0] || a->min[0] > b->max[0]) return false;
        if (a->max[1] < b->min[1] || a->min[1] > b->max[1]) return false;
        return !(a->max[2] < b->min[2] || a->min[2] > b->max[2]);

    }
};



class RigidObjectModel{

private:
    GLfloat * vertexArray;
    unsigned arraySize;
    std::vector<Eigen::Vector3f> pointArray;

    std::vector<GLfloat> green;
    std::vector<glm::vec3> boundingBoxCornersForPrint;

    static bool compareFloats (const float A, const float B){
        float diff = B - A;
        return (diff < 0.001) ; //&& (-diff < 0.01);
    }

    struct lex_compare {
        bool operator() (const Eigen::Vector3f& lhs, const Eigen::Vector3f& rhs) const {
            if(lhs(0) < rhs(0) && !compareFloats(lhs(0), rhs(0)))
                return true;
            if(lhs(1) < rhs(1)  && !compareFloats(lhs(1), rhs(1)))
                return true;
            if(lhs(2) , rhs(2)  && !compareFloats(lhs(2), rhs(2)))
                return true;
            return false;
        }
    };

    void createPointArray(){
        std::set<Eigen::Vector3f, lex_compare> s;

        // create point array from triangles buffer
        for(int k = 0 ; (int)k < arraySize - 2; k+=3){
            Eigen::Vector3f x;
            x(0) = * (vertexArray + k );
            x(1) = vertexArray[k+1];
            x(2) = vertexArray[k+2];
            s.emplace(x);
        }

        std::copy(s.begin(), s.end(), std::back_inserter(pointArray));
    }

    Eigen::Matrix3f covarianceMatrixFromConvex( std::vector<Eigen::Vector3f> & pnts){
        // INIT
        Eigen::Vector3f mu;
        Eigen::Matrix3f C;
        for(int i = 0 ; i < 3 ; i++){
            mu(i) = 0.0f;
            for(int j = 0 ; j < 3 ; j++){
                C(i,j) = 0.0f;
            }
        }

        // Calculate Means
        auto cntPoints = float(pnts.size()) ;
        for(auto & pnt : pnts ){
            mu[0] += pnt(0) / cntPoints;
            mu[1] += pnt(1) / cntPoints;
            mu[2] += pnt(2) / cntPoints;
        }

        // Build covariance matrix
        for(auto & p : pnts ){
            for(int i = 0 ; i < 3 ; ++i){
                for(int j = 0 ; j < 3 ; j++) {
                    C(i,j) += p(i)*p(j) - mu(i)*mu(j);
                }
            }
        }

        return C;
    }

    glm::vec3 Min( glm::vec3 x, glm::vec3 in ){ return glm::vec3( std::min(x.x,in.x), std::min(x.y,in.y), std::min(x.z,in.z) ); }
    glm::vec3 Max( glm::vec3 x, glm::vec3 in ){ return glm::vec3( std::max(x.x,in.x), std::max(x.y,in.y), std::max(x.z,in.z) ); }

    void calculate(){
        // Conariance matrix
        Eigen::Matrix3f PCA = covarianceMatrixFromConvex(pointArray);

        // Eigen vectors
        Eigen::EigenSolver<Eigen::MatrixXf> eigensolver;
        eigensolver.compute(PCA);
        Eigen::MatrixXf eigen_vectors = eigensolver.eigenvectors().real();
        glm::vec3 R, U, F;
        for(int i=0; i< 3; i++){
            R[i] = eigen_vectors.row(0)(i);
            U[i] = eigen_vectors.row(1)(i);
            F[i] = eigen_vectors.row(2)(i);
        }
        R = glm::normalize(R);
        U = glm::normalize(U);
        F = glm::normalize(F);

        // now build the bounding box extents in the rotated frame
        glm::vec3 minim(1e10, 1e10, 1e10), maxim(-1e10, -1e10, -1e10);
        for(auto & i : pointArray){
            glm::vec3 p(i(0), i(1), i(2));
            glm::vec3 p_prime( glm::dot(R, p), glm::dot(U, p), glm::dot(F, p) );
            minim = Min(minim, p_prime);
            maxim = Max(maxim, p_prime);
        }

        // ROTATION MATRIX
        rotation_r[0][0]=R.x; rotation_r[0][1]=U.x; rotation_r[0][2]=F.x; // r1 u1 f1
        rotation_r[1][0]=R.y; rotation_r[1][1]=U.y; rotation_r[1][2]=F.y; // r2 u2 f2
        rotation_r[2][0]=R.z; rotation_r[2][1]=U.z; rotation_r[2][2]=F.z; // r3 u3 f3

        // EXTENSION
        extents_e = (maxim - minim)/2.0f;;

        // POINT CENTER
        glm::vec3 point_center = (maxim + minim)/2.0f;
        position_u[0] = glm::dot(rotation_r[0] , point_center);
        position_u[1] = glm::dot(rotation_r[1] , point_center);
        position_u[2] = glm::dot(rotation_r[2] , point_center);
    }

public:
    // FOR obb
    glm::vec3 rotation_r[3]; // obb center point ... 3
    glm::vec3 position_u;  // Local x-, y-, and z-axes   // maybe float array .. jop 3 body ... 9
    glm ::vec3 extents_e; // Positive halfwidth extents of obb

    RigidObjectModel(const GLfloat * vertexArray, unsigned arraySize){
        this->vertexArray = new  GLfloat [arraySize];
        for(unsigned i = 0 ; i < arraySize ; ++i)
            this->vertexArray[i] = vertexArray[i];

        this->arraySize = arraySize;
        createPointArray();
        calculate();
    }
    const GLfloat * getVertexArray(){
        return vertexArray;
    }
    unsigned getVertexArraySize(){
        return arraySize;
    }
    const std::vector<Eigen::Vector3f> & getPointArray(){
        return pointArray;
    }
};

class OBB{

private:
    RigidObjectModel * objectModel;
    std::vector<GLfloat> green;
    long unsigned arraySize = 288;
    std::vector<glm::vec3> boundingBoxCornersForPrint;

    // (No calculation actually...) Calculate Word coords of the corners from Rotation Matrix R, Center C and extends E
    void createCornersFrom_R_C_E(){
        boundingBoxCornersForPrint.clear();
        glm::vec3 R;
        glm::vec3 U;
        glm::vec3 F;
        R.x = rotation_r[0][0]; U.x = rotation_r[0][1]; F.x = rotation_r[0][2]; // r1 u1 f1
        R.y = rotation_r[1][0]; U.y = rotation_r[1][1]; F.y = rotation_r[1][2]; // r2 u2 f2
        R.z = rotation_r[2][0]; U.z = rotation_r[2][1]; F.z = rotation_r[2][2]; // r3 u3 f3

        // vzdy zacnu v centru - a posunu se o polmer do kazdeho ze smeru
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u - R * extent_e[0] - U * extent_e[1] - F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u + R * extent_e[0] - U * extent_e[1] - F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u + R * extent_e[0] - U * extent_e[1] + F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u - R * extent_e[0] - U * extent_e[1] + F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u - R * extent_e[0] + U * extent_e[1] - F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u + R * extent_e[0] + U * extent_e[1] - F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u + R * extent_e[0] + U * extent_e[1] + F * extent_e[2]));
        this->boundingBoxCornersForPrint.emplace_back( glm::vec3(center_u - R * extent_e[0] + U * extent_e[1] + F * extent_e[2]));
    }

    // (No calculation actually...) Return bouding box corners for print
    std::vector<GLfloat> linesFromCorners()  {

        createCornersFrom_R_C_E();

        std::vector<GLfloat> list;
        for(int i = 0 ; i < 3; ++i){ /// A--B--C--D
            // start edge
            list.emplace_back(boundingBoxCornersForPrint[i][0]);
            list.emplace_back(boundingBoxCornersForPrint[i][1]);
            list.emplace_back(boundingBoxCornersForPrint[i][2]);

            // end edge
            list.emplace_back(boundingBoxCornersForPrint[i + 1][0]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][1]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][2]);
        }

        /// D--A
        // start edge
        list.emplace_back(boundingBoxCornersForPrint[0][0]);
        list.emplace_back(boundingBoxCornersForPrint[0][1]);
        list.emplace_back(boundingBoxCornersForPrint[0][2]);

        // end edge
        list.emplace_back(boundingBoxCornersForPrint[3][0]);
        list.emplace_back(boundingBoxCornersForPrint[3][1]);
        list.emplace_back(boundingBoxCornersForPrint[3][2]);


        for(int i = 4 ; i < 7; ++i){ /// E--F--G--H
            // start edge
            list.emplace_back(boundingBoxCornersForPrint[i][0]);
            list.emplace_back(boundingBoxCornersForPrint[i][1]);
            list.emplace_back(boundingBoxCornersForPrint[i][2]);

            // end edge
            list.emplace_back(boundingBoxCornersForPrint[i + 1][0]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][1]);
            list.emplace_back(boundingBoxCornersForPrint[i + 1][2]);
        }

        /// H--E
        // start edge
        list.emplace_back(boundingBoxCornersForPrint[7][0]);
        list.emplace_back(boundingBoxCornersForPrint[7][1]);
        list.emplace_back(boundingBoxCornersForPrint[7][2]);

        // end edge
        list.emplace_back(boundingBoxCornersForPrint[4][0]);
        list.emplace_back(boundingBoxCornersForPrint[4][1]);
        list.emplace_back(boundingBoxCornersForPrint[4][2]);

        for(int i = 0 ; i < 4; ++i){ /// A---E ; B---F ; C---G; D---H  // vertical lines
            // start edge
            list.emplace_back(boundingBoxCornersForPrint[i][0]);
            list.emplace_back(boundingBoxCornersForPrint[i][1]);
            list.emplace_back(boundingBoxCornersForPrint[i][2]);

            // end edge
            list.emplace_back(boundingBoxCornersForPrint[i + 4][0]);
            list.emplace_back(boundingBoxCornersForPrint[i + 4][1]);
            list.emplace_back(boundingBoxCornersForPrint[i + 4][2]);
        }
        return list;
    }

public:
    glm::vec3 rotation_r[3];
    glm::vec3 center_u;    // 1 point
    glm ::vec3 extent_e;   // Positive halfwidth extents of obb

     RigidObjectModel * getObjectModel() const {
        return objectModel;
    }

    bool intersects(const OBB * b) const {

        if(this == b)
            return false;

      const OBB * a = this;

        float ra, rb;
        Eigen::Matrix3f R, AbsR;

        // Compute rotation matrix expressing B in a’s coordinate frame
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                R(i,j) = glm::dot(a->rotation_r[i], b->rotation_r[j]); // udelany sloupecky  // TODO -- mozna to pri rotacich zacne bugovat
//                glm::vec3 ar(a->rotation_r[0][i], a->rotation_r[1][i], a->rotation_r[2][i]);
//                glm::vec3 br(b->rotation_r[0][j], b->rotation_r[1][j], b->rotation_r[2][j]);
//                R(i,j) = glm::dot(ar, br); // udelany sloupecky
            }


        // Compute translation vector t == to je ok
        glm::vec3 t = b->center_u - a->center_u;

        // Bring translation into a’s coordinate frame
//        glm::vec3 aR(a->rotation_r[0][0], a->rotation_r[1][0], a->rotation_r[2][0]);
//        glm::vec3 aU(a->rotation_r[0][1], a->rotation_r[1][1], a->rotation_r[2][1]);
//        glm::vec3 aF(a->rotation_r[0][2], a->rotation_r[1][2], a->rotation_r[2][2]);
//        t = glm::vec3(glm::dot(t, aR), glm::dot(t, aU), glm::dot(t, aF));
        t = glm::vec3(glm::dot(t, a->rotation_r[0]), glm::dot(t, a->rotation_r[1]), glm::dot(t, a->rotation_r[2]));

        // Compute common subexpressions. Add in an epsilon term to
        // counteract arithmetic errors when two edges are parallel and
        // their cross product is (near) null (see text for details)
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                AbsR(i,j) = fabs(R(i,j)) + 0.00001f; // todo - epsilon + 0.01;

        // Test axes L = A0, L = A1, L = A2
        for (int i = 0; i < 3; i++) {
            ra = a->extent_e[i];
            rb = b->extent_e[0] * AbsR(i, 0) + b->extent_e[1] * AbsR(i, 1) + b->extent_e[2] * AbsR(i, 2);
            if (fabs(t[i]) > ra + rb) return false;
        }
        // Test axes L = B0, L = B1, L = B2
        for (int i = 0; i < 3; i++) {
            ra = a->extent_e[0] * AbsR(0, i) + a->extent_e[1] * AbsR(1, i) + a->extent_e[2] * AbsR(2, i);
            rb = b->extent_e[i];
            if (fabs(t[0] * R(0,i) + t[1] * R(1,i) + t[2] * R(2,i)) > ra + rb) return false;
        }

        // Test axis L = A0 x B0
        ra = a->extent_e[1] * AbsR(2,0) + a->extent_e[2] * AbsR(1,0);
        rb = b->extent_e[1] * AbsR(0,2) + b->extent_e[2] * AbsR(0,1);
        if (fabs(t[2] * R(1,0) - t[1] * R(2,0)) > ra + rb) return false;
        // Test axis L = A0 x B1
        ra = a->extent_e[1] * AbsR(2,1) + a->extent_e[2] * AbsR(1,1);
        rb = b->extent_e[0] * AbsR(0,2) + b->extent_e[2] * AbsR(0,0);
        if (fabs(t[2] * R(1,1) - t[1] * R(2,1)) > ra + rb) return false;
        // Test axis L = A0 x B2
        ra = a->extent_e[1] * AbsR(2,2) + a->extent_e[2] * AbsR(1,2);
        rb = b->extent_e[0] * AbsR(0,1) + b->extent_e[1] * AbsR(0,0);
        if (fabs(t[2] * R(1,2) - t[1] * R(2,2)) > ra + rb) return false;
        // Test axis L = A1 x B0
        ra = a->extent_e[0] * AbsR(2,0) + a->extent_e[2] * AbsR(0,0);
        rb = b->extent_e[1] * AbsR(1,2) + b->extent_e[2] * AbsR(1,1);
        if (fabs(t[0] * R(2,0) - t[2] * R(0,0)) > ra + rb) return false;
        // Test axis L = A1 x B1
        ra = a->extent_e[0] * AbsR(2,1) + a->extent_e[2] * AbsR(0,1);
        rb = b->extent_e[0] * AbsR(1,2) + b->extent_e[2] * AbsR(1,0);
        if (fabs(t[0] * R(2,1) - t[2] * R(0,1)) > ra + rb) return false;
        // Test axis L = A1 x B2
        ra = a->extent_e[0] * AbsR(2,2) + a->extent_e[2] * AbsR(0,2);
        rb = b->extent_e[0] * AbsR(1,1) + b->extent_e[1] * AbsR(1,0);
        if (fabs(t[0] * R(2,2) - t[2] * R(0,2)) > ra + rb) return false;
        // Test axis L = A2 x B0
        ra = a->extent_e[0] * AbsR(1,0) + a->extent_e[1] * AbsR(0,0);
        rb = b->extent_e[1] * AbsR(2,2) + b->extent_e[2] * AbsR(2,1);
        if (fabs(t[1] * R(0,0) - t[0] * R(1,0)) > ra + rb) return false;
        // Test axis L = A2 x B1
        ra = a->extent_e[0] * AbsR(1,1) + a->extent_e[1] * AbsR(0,1);
        rb = b->extent_e[0] * AbsR(2,2) + b->extent_e[2] * AbsR(2,0);
        if (fabs(t[1] * R(0,1) - t[0] * R(1,1)) > ra + rb) return false;
        // Test axis L = A2 x B2
        ra = a->extent_e[0] * AbsR(1,2) + a->extent_e[1] * AbsR(0,2);
        rb = b->extent_e[0] * AbsR(2,1) + b->extent_e[1] * AbsR(2,0);
        if (fabs(t[1] * R(0,2) - t[0] * R(1,2)) > ra + rb) return false;

        // Since no separating axis is found, the OBBs must be intersecting
//        std::cout << center_u.x << " " << center_u.y << " " <<  center_u.z << std::endl;
//        std::cout << b->center_u.x << " " << b->center_u.y << " " <<  b->center_u.z << std::endl << std::endl;
        return true;
    }

    void renderBoundingArea(GLuint vertexBuffer, GLuint colorbuffer, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix, GLuint shaderProgramID, GLuint shaderMVPMatrixID) {

        std::vector<GLfloat> boundingBoxLines = linesFromCorners(); // allready in the Word coords

        Loader::loadObject(vertexBuffer, colorbuffer, &boundingBoxLines[0], &(green[0]), (int)arraySize); // to zni OK

        // MVP
        glm::mat4 mvp = projectionMatrix * viewMatrix;

        // load shader && bind MVP to it
        glUseProgram(shaderProgramID);
        glUniformMatrix4fv(shaderMVPMatrixID, 1, GL_FALSE, &mvp[0][0]);

        // draw
        Loader::render(vertexBuffer, colorbuffer, 0, 12*2, false);
    }

    explicit OBB(std::vector<GLfloat> & pointArray) : green(myObject::cubeOutlineColorGreen, myObject::cubeOutlineColorGreen + 72) {
        // Calculates various thing like covariance matrix etc...
        objectModel = new RigidObjectModel(&(pointArray[0]), pointArray.size());

        // Points in model space - Collected from the model
        rotation_r[0] = objectModel->rotation_r[0];
        rotation_r[1] = objectModel->rotation_r[1];
        rotation_r[2] = objectModel->rotation_r[2];
        center_u = objectModel->position_u;
        extent_e = objectModel->extents_e;

        createCornersFrom_R_C_E();
    }
};

#endif //OPENGL_GAME_BOUNDINGAREA_HPP
