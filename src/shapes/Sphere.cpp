#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!


    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, topLeft);

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, bottomLeft);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, bottomRight);

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, bottomRight);

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, topRight);

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, topLeft);
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!
    float thetaStep = glm::radians(180.f/m_param1);
    float topPhi = 0.f;
    float bottomPhi = topPhi + thetaStep;
    for(int i = 0; i < m_param1; i++){

        glm::vec3 TL = {sin(topPhi) * sin(currentTheta), cos(topPhi), sin(topPhi) * cos(currentTheta)};
        glm::vec3 TR = {sin(topPhi) * sin(nextTheta), cos(topPhi), sin(topPhi) * cos(nextTheta)};
        glm::vec3 BL = {sin(bottomPhi) * sin(currentTheta), cos(bottomPhi), sin(bottomPhi) * cos(currentTheta)};
        glm::vec3 BR = {sin(bottomPhi) * sin(nextTheta), cos(bottomPhi), sin(bottomPhi) * cos(nextTheta)};

        makeTile(TL, TR, BL, BR);

        topPhi = bottomPhi;
        bottomPhi += thetaStep;


    }

}

void Sphere::makeSphere() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep = glm::radians(360.f/m_param2);
    float currTheta = 0.f;
    for(int i = 0; i < m_param2; i++){
        makeWedge(currTheta, currTheta+thetaStep);
        currTheta += thetaStep;
    }
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:


    //param2 = wedges
    //param1 = phi


    // Uncomment these lines to make sphere for Task 7:
     m_param1 = m_param1 < 2 ? 2: m_param1;
     m_param2 = m_param2 < 3 ? 3: m_param2;
     makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}