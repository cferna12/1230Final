#include "Camera.h"


/*Updates view matrix with new camera data*/
glm::mat4 Camera::updateViewMat(SceneCameraData cam_data){
    glm::vec3 u;
    glm::vec3 v;
    glm::vec3 w;

    glm::vec3 look = glm::vec3(cam_data.look);
    glm::vec3 up = glm::vec3(cam_data.up);
    glm::vec3 pos = glm::vec3(cam_data.pos);

    w = -look;
    w = glm::normalize(w);

    v = up - glm::dot(up, w)*w;

    v = glm::normalize(v);
    u = glm::cross(v, w);

    glm::mat4 Mr = glm::mat4(u[0], v[0], w[0], 0, u[1], v[1], w[1], 0, u[2], v[2], w[2], 0,0,0,0,1);
    glm::mat4 Mt = glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0, -pos[0], -pos[1], -pos[2],1);


    return Mr*Mt;
}


/*Updates projection matrix with new values*/
glm::mat4 Camera::updateProjMat(float near, float far, float width, float height, float height_angle){
    glm::mat4 translate = {1,0,0,0,
                          0,1,0,0,
                          0,0,-2,0,
                          0,0,-1,1};
    float c = (-near)/far;
    glm::mat4 unhinge = {1,0,0,0,
                        0,1,0,0,
                        0,0, (1/(1+c)), -1,
                        0,0, (-c/(1+c)), 0};





    float L_h = tan(height_angle/2)*far;
    float L_w = (L_h*width)/height;
    float aspect = width/height;

    float width_angle_2 = atan(tan(height_angle/2) * aspect);
    float checkLw = tan(width_angle_2)* far;
//    float width_angle =
    glm::mat4 scale = {(1/L_w), 0,0,0,
                      0, 1/L_h, 0,0,
                      0,0,1/far, 0,
                      0,0,0,1};


   return (translate*unhinge*scale);
}
