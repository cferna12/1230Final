#include "Cone.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


//used to tesselate bottom face, same code from cube and cylinder
void Cone::makeBottomTiles(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.

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

/*Makes a tile with normals used in implicit equation*/
void Cone::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.

    glm::vec3 n = glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight-topLeft));
    insertVec3(m_vertexData, topLeft);

    n = 2.f*topLeft;
    n[1] = 0.25 - 0.5*topLeft[1];
    insertVec3(m_vertexData, glm::normalize(n));

    insertVec3(m_vertexData, bottomLeft);
    n = 2.f*bottomLeft;
    n[1] = 0.25 - 0.5*bottomLeft[1];
    insertVec3(m_vertexData, glm::normalize(n));

    insertVec3(m_vertexData, bottomRight);
    n = 2.f*bottomRight;
    n[1] = 0.25 - 0.5*bottomRight[1];
    insertVec3(m_vertexData, glm::normalize(n));

//    glm::vec3 n1 =glm::normalize(glm::cross(topRight - bottomRight, topLeft-bottomRight));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(n));

    insertVec3(m_vertexData, topRight);
    n = 2.f*topRight;
    n[1] = 0.25 - 0.5*topRight[1];
    insertVec3(m_vertexData, glm::normalize(n));

    insertVec3(m_vertexData, topLeft);
    n = 2.f*topLeft;
    n[1] = 0.25 - 0.5*topLeft[1];
    insertVec3(m_vertexData, glm::normalize(n));


}

void Cone::makeWedge(float currentTheta, float nextTheta) {

    float r = m_radius;
    float y = 0.5;


    float x1 = r*cos(currentTheta);
    float z1 = r*sin(currentTheta);

    float x2 = r*cos(nextTheta);
    float z2 = r*sin(nextTheta);

    //make bottom base case triangle
    float tri_x1 = (x1/m_param1);
    float tri_z1 = (z1/m_param1);
    float tri_x2 = (x2/m_param1);
    float tri_z2 = (z2/m_param1);
    glm::vec3 v1 = {tri_x1, -0.5, tri_z1};
    glm::vec3 v2 = {0, -0.5, 0};
    glm::vec3 v3 = {tri_x2, -0.5, tri_z2};
    makeTriangle(v3, v2, v1, {0,-1,0});


    glm::vec3 vv1 = {0,0.5,0};
    glm::vec3 vv2 = {0,0.5,0};
    glm::vec3 vv3 = {0,0.5,0};



    //create base triangle
    for(int i = 0; i < 1; i++){
        float new_y = 0.5f - (1/(float) m_param1);
        float new_r = (0.5 - new_y)/2;
        float x1 = new_r*cos(currentTheta);
        float z1 = new_r*sin(currentTheta);

        float x2 = new_r*cos(nextTheta);
        float z2 = new_r*sin(nextTheta);

        vv1 = {x2, new_y, z2};
        vv3 = {x1, new_y, z1};

        float tip_theta = (currentTheta + nextTheta)/2;
        float x_tip_norm = cos(tip_theta);
        float z_tip_norm = sin(tip_theta);
        makeTipTriangle(vv3, vv2, vv1, {x_tip_norm, 0.5, z_tip_norm});
        y = new_y;
        r = new_r;
    }

    for(int i = 1; i < m_param1; i++){
        /*Makes side tiles of cone*/
        float next_y = y - (1.f)/(float) m_param1;
        float next_r = (0.5 - next_y)/2;

        glm::vec3 TR = {r*cos(currentTheta), y, r*sin(currentTheta)};
        glm::vec3 TL = {r*cos(nextTheta), y, r*sin(nextTheta)};
        glm::vec3 BR = {next_r*cos(currentTheta), next_y, next_r*sin(currentTheta)};
        glm::vec3 BL = {next_r*cos(nextTheta), next_y, next_r*sin(nextTheta)};
        makeTile(TL, TR, BL, BR);


        /*Make bottom cap*/
        glm::vec3 v2 =   {tri_x1*(i+1), 0.5, tri_z1*(i+1)};
        glm::vec3 v4 =   {tri_x2*(i+1), 0.5, tri_z2*(i+1)};
        makeBottomTiles({v1[0], -0.5, v1[2]}, {v3[0], -0.5, v3[2]}, {v2[0], -0.5, v2[2]}, {v4[0], -0.5, v4[2]});


        //update vars
        v1 = v2;
        v3 = v4;
        y = next_y;
        r = next_r;

    }



}

//makes triangle with inputted vertices and normal (used for top and bottom caps)
void Cone::makeTriangle(glm::vec3 topRight,
                            glm::vec3 topLeft,
                            glm::vec3 bottomLeft, glm::vec3 normal){

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);

}

//called for cone tip triangles and uses calculated tip normals
void Cone::makeTipTriangle(glm::vec3 topRight,
                   glm::vec3 topLeft,
                   glm::vec3 bottomLeft, glm::vec3 tip_norm){

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, {2*topRight[0], 0.25 - 0.5*topRight[1], 2*topRight[2]});

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::vec3{tip_norm[0], -0.5 + 2*abs(topLeft[1]), tip_norm[2]}));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, {2*bottomLeft[0], 0.25 - 0.5*bottomLeft[1], 2*bottomLeft[2]});

}


void Cone::makeCone() {

    float thetaStep = glm::radians(360.f/m_param2);
    float currTheta = 0.f;
    for(int i = 0; i < m_param2; i++){
        makeWedge(currTheta, currTheta+thetaStep);
        currTheta += thetaStep;
    }
}

void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    m_param2 = m_param2 < 3 ? 3: m_param2;
    makeCone();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
