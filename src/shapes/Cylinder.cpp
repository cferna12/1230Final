#include "Cylinder.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

//same code from Cube for inserting tile vertices and normals
void Cylinder::makeCubeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {

    glm::vec3 n = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight-topLeft));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n);

    glm::vec3 n1 =glm::normalize(glm::cross(topRight - bottomRight, topLeft-bottomRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, n1);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, n1);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n1);


}

/*Makes tile with normals pointing away from position*/
void Cylinder::makeCylinderTile(glm::vec3 topLeft,
                         glm::vec3 topRight,
                         glm::vec3 bottomLeft,
                         glm::vec3 bottomRight){
    glm::vec3 n = topLeft;
    n[1] = 0;
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, bottomLeft);
    n = bottomLeft;
    n[1] = 0;
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, bottomRight);
    n = bottomRight;
    n[1] = 0;
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, bottomRight);
    n = bottomRight;
    n[1] = 0;
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, topRight);
    n = topRight;
    n[1] = 0;
    insertVec3(m_vertexData, n);

    insertVec3(m_vertexData, topLeft);
    n = topLeft;
    n[1] = 0;
    insertVec3(m_vertexData, n);


}

//makes triangle with inputted vertices and normal (used for top and bottom caps)
void Cylinder::makeTriangle(glm::vec3 topRight,
                            glm::vec3 topLeft,
                            glm::vec3 bottomLeft, glm::vec3 normal){

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);

}


void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    float r = 0.5;
    float y = 0.5;

    float x1 = r*cos(currentTheta);
    float z1 = r*sin(currentTheta);

    float x2 = r*cos(nextTheta);
    float z2 = r*sin(nextTheta);

    //MAKE BASE CASE triangles
    float tri_x1 = (x1/m_param1);
    float tri_z1 = (z1/m_param1);
    float tri_x2 = (x2/m_param1);
    float tri_z2 = (z2/m_param1);
    glm::vec3 v1 = {tri_x1, 0.5, tri_z1};
    glm::vec3 v2 = {0, 0.5, 0};
    glm::vec3 v3 = {tri_x2, 0.5, tri_z2};
    makeTriangle(v1, v2, v3, glm::vec3{0,1,0});
    makeTriangle({v3[0], -0.5, v3[2]}, {v2[0], -0.5, v2[2]}, {v1[0], -0.5, v1[2]}, glm::vec3{0,-1,0});

    for(int i = 0; i < m_param1; i++){
        /*Makes side tiles of cylinder*/
        float next_y = y - (1.f)/(float) m_param1;
        glm::vec3 TR = {x1, y, z1};
        glm::vec3 TL = {x2, y, z2};
        glm::vec3 BR = {x1, next_y, z1};
        glm::vec3 BL = {x2, next_y, z2};
        makeCylinderTile(TL, TR, BL, BR);

        /*Make top cap*/
        glm::vec3 v2 =   {tri_x1*(i+1), 0.5, tri_z1*(i+1)};
        glm::vec3 v4 =   {tri_x2*(i+1), 0.5, tri_z2*(i+1)};
        makeCubeTile(v3, v1, v4, v2);

        //make bottom cap
        makeCubeTile({v1[0], -0.5, v1[2]}, {v3[0], -0.5, v3[2]}, {v2[0], -0.5, v2[2]}, {v4[0], -0.5, v4[2]});

        //update vertices
        v1 = v2;
        v3 = v4;
        y = next_y;
    }
}

void Cylinder::makeCylinder() {
    float thetaStep = glm::radians(360.f/m_param2);
    float currTheta = 0.f;
    for(int i = 0; i < m_param2; i++){
        makeWedge(currTheta, currTheta+thetaStep);
        currTheta += thetaStep;
    }
}

void Cylinder::setVertexData() {
    //clamp param2 to minimum
    m_param2 = m_param2 < 3 ? 3: m_param2;
    makeCylinder();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
