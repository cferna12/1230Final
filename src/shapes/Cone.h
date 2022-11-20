#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeWedge(float currentTheta, float nextTheta);
    void makeCone();
    void makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight);
    std::vector<float> m_vertexData;
    void makeTriangle(glm::vec3 topRight,
                                glm::vec3 topLeft,
                                glm::vec3 bottomLeft, glm::vec3 normal);
    void makeTipTriangle(glm::vec3 topRight,
                       glm::vec3 topLeft,
                       glm::vec3 bottomLeft,  glm::vec3 tip_norm);
    void makeBottomTiles(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight) ;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
