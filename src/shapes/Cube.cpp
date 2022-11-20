#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
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

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube

    int index;
    float square_size = 1/((float) m_param1);

    //if x face
    if(topLeft[0] == topRight[0] && topLeft[0] == bottomLeft[0]){
        glm::vec3 holder;
        float neg = 1;

        if(topLeft[0] ==  -0.5){
            holder = topLeft;
            topLeft = topRight;
            topRight = holder;
            holder = bottomLeft;
            bottomLeft = bottomRight;
            bottomRight = holder;
            neg = -1;
        }

        for(int row = 0; row < m_param1; row++){
            for(int col = 0; col < m_param1; col++){
                auto leftY = (topLeft[1] - square_size*row);

                auto leftZ = (topLeft[2] - neg*square_size*col);
                makeTile({topLeft[0], leftY, leftZ}, {topLeft[0], leftY, leftZ - neg*square_size},
                         {topLeft[0], leftY - square_size, leftZ}, {topLeft[0], leftY- square_size, leftZ - neg*square_size});
            }
        }
    }


    //if y face
    else if(topLeft[1] == topRight[1] && topLeft[1] == bottomLeft[1]){
        glm::vec3 holder;
        float neg = 1;

        if(topLeft[1] == -0.5){
            holder = topLeft;
            topLeft = topRight;
            topRight = holder;
            holder = bottomLeft;
            bottomLeft = bottomRight;
            bottomRight = holder;
            neg = -1;
        }

        for(int row = 0; row < m_param1; row++){
            for(int col = 0; col < m_param1; col++){
                auto leftX = (topLeft[0] + neg*square_size*col);

                auto leftZ = (topLeft[2] + square_size*row);
                makeTile({leftX, topLeft[1], leftZ}, {leftX + neg*square_size, topLeft[1], leftZ},
                         {leftX, topLeft[1], leftZ +  square_size}, {leftX + neg*square_size, topLeft[1], leftZ + square_size});
            }
        }
    }



    //if Z face
    else{
        index = 2;
        glm::vec3 holder;
        float neg = 1;

        if(topLeft[2] == -0.5){
            holder = topLeft;
            topLeft = topRight;
            topRight = holder;
            holder = bottomLeft;
            bottomLeft = bottomRight;
            bottomRight = holder;
            neg = -1;
        }

        for(int row = 0; row < m_param1; row++){
            for(int col = 0; col < m_param1; col++){
                auto leftX = (topLeft[0] + neg*square_size*col);

                auto leftY = (topLeft[1] - square_size*row);
                makeTile({leftX, leftY, topLeft[2]}, {leftX + square_size*neg, leftY, topRight[2]},
                         {leftX, leftY - square_size, bottomLeft[2]}, {leftX + neg*square_size, leftY - square_size, bottomRight[2]});
            }
        }
    }


}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

//     makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
//              glm::vec3( 0.5f,  0.5f, 0.5f),
//              glm::vec3(-0.5f, -0.5f, 0.5f),
//              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:


    //front face
     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));

    //back face
     makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f));

     //top
     makeFace(glm::vec3(-0.5f,  0.5f,  -0.5f),
              glm::vec3(0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f,  0.5f),
              glm::vec3(0.5f,  0.5f, 0.5f));
      //bottom
     makeFace(glm::vec3(-0.5f,  -0.5f,  -0.5f),
              glm::vec3(0.5f,  -0.5f, -0.5f),
              glm::vec3(-0.5f,  -0.5f,  0.5f),
              glm::vec3(0.5f,  -0.5f, 0.5f));

     //right
     makeFace(glm::vec3(0.5f,  0.5f,  0.5f),
              glm::vec3(0.5f,  0.5f, -0.5f),
              glm::vec3(0.5f,  -0.5f,  0.5f),
              glm::vec3(0.5f,  -0.5f, -0.5f));

     //left
     makeFace(glm::vec3(-0.5f,  0.5f,  0.5f),
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  -0.5f,  0.5f),
              glm::vec3(-0.5f,  -0.5f, -0.5f));
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
