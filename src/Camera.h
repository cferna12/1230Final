#pragma once

#include <glm/glm.hpp>
#include "./utils/scenedata.h"

class Camera{
public:

    static glm::mat4 updateViewMat(SceneCameraData cam_data);
    static glm::mat4 updateProjMat(float near, float far, float width, float height, float height_angle);

private:
    float height_angle;
    float world_pos;
};
