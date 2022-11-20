#include "sceneparser.h"
#include "scenefilereader.h"
#include <glm/gtx/transform.hpp>

#include <chrono>
#include <iostream>


void recur(SceneNode *node, glm::mat4 CTM, RenderData &renderData){ //passing correctly?
    for(int i = 0; i < node->transformations.size(); i++){
        SceneTransformation *trans = node->transformations[i];
        glm::mat4 t;
        switch(trans->type){
            case TransformationType::TRANSFORMATION_TRANSLATE:
                t = glm::translate(trans->translate);
            break;
            case TransformationType::TRANSFORMATION_ROTATE:
                t = glm::rotate(trans->angle, trans->rotate);
            break;
            case TransformationType::TRANSFORMATION_SCALE:
                t = glm::scale(trans->scale);
            break;
            case TransformationType::TRANSFORMATION_MATRIX:
                t *= trans->matrix;
            break;
        }

        CTM *= t;
    }

    if(node->primitives.size() > 0){
        for(int i = 0 ; i < node->primitives.size(); i++){
            RenderShapeData data = RenderShapeData{*node->primitives[i], CTM};
            renderData.shapes.push_back(data);
        }
    }


    for(int i = 0; i < node->children.size(); i ++){
       recur(node->children[i], CTM, renderData);
    }
}

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }


    renderData.globalData = fileReader.getGlobalData();
    renderData.cameraData= fileReader.getCameraData();
    renderData.lights = fileReader.getLights();

    SceneNode *root = fileReader.getRootNode();
    renderData.shapes.clear();
    glm::mat4 CTM( 1.0f );
    recur(root, CTM, renderData);


    return true;
}


