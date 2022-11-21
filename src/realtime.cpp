#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "glm/ext/matrix_transform.hpp"
#include "settings.h"
#include "utils/sceneparser.h"
#include "utils/shaderloader.h"
#include "Camera.h"
#include "shapes/Sphere.h"
#include "shapes/Cube.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include "glm/ext/matrix_clip_space.hpp"

// ================== Project 5: Lights, Camera


bool done = false;
Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");

    /*save initial settings*/
    curr_p1 = settings.shapeParameter1;
    curr_p2 = settings.shapeParameter2;
    curr_near = settings.nearPlane;
    curr_far = settings.farPlane;

    cube_data.updateParams(curr_p1);
    sphere_data.updateParams(curr_p1, curr_p2);
    cyl_data.updateParams(curr_p1, curr_p2);
    cone_data.updateParams(curr_p1, curr_p2);

    //reserve space for IDs
    VBOS.reserve(4);
    VAOS.reserve(4);
    shape_data_sizes.reserve(4);

    /*Start of vbo generation for base shapes*/
    std::vector<std::vector<float>> primitives = {sphere_data.generateShape(), cube_data.generateShape(), cyl_data.generateShape(), cone_data.generateShape()};
    for(int i = 0; i < 4; i++){
        glGenBuffers(1, &VBOS[i]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOS[i]);
        auto shape_data = primitives[i];
        shape_data_sizes.push_back(shape_data.size());
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*shape_data.size(), shape_data.data(), GL_STATIC_DRAW);

        // Generate, and bind vao
        glGenVertexArrays(1, &VAOS[i]);
        glBindVertexArray(VAOS[i]);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

        // Clean-up bindings
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }

    //set done to true once initialization complete
    done = true;
}

/**
 * @brief Realtime::updateBuffers
 * Loops through each vbos and sends the updated vertex data for each shape
 */
void Realtime::updateBuffers(){
    std::vector<std::vector<float>> primitives = {sphere_data.generateShape(), cube_data.generateShape(), cyl_data.generateShape(), cone_data.generateShape()};
    for(int i = 0; i < 4; i++){
        glBindBuffer(GL_ARRAY_BUFFER, VBOS[i]);
        auto shape_data = primitives[i];
        shape_data_sizes[i] = shape_data.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*shape_data.size(), shape_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
}


void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint curr_vao_id = 0;
    int size_index = 0;

    glm::mat4 curr_CTM;
    glm::mat3 curr_ITCTM;

    /*Loop through shapes in scene and paint each*/
    for(auto shape: m_metaData.shapes){
        switch(shape.primitive.type){
            case PrimitiveType::PRIMITIVE_SPHERE:
                curr_vao_id = VAOS[0];
                size_index = 0;
                break;
            case PrimitiveType::PRIMITIVE_CUBE:
                curr_vao_id = VAOS[1];
                size_index = 1;
                break;
            case PrimitiveType::PRIMITIVE_CYLINDER:
                curr_vao_id = VAOS[2];
                size_index = 2;
                break;
            case PrimitiveType::PRIMITIVE_CONE:
                curr_vao_id = VAOS[3];
                size_index = 3;
                break;
        }


        curr_CTM = shape.ctm;
        curr_ITCTM = shape.inv_transpose;
        glBindVertexArray(curr_vao_id);

        //Activate shader
        glUseProgram(m_shader);

        /*Send shape matrices*/
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_model"), 1, GL_FALSE, &curr_CTM[0][0]);
        glUniformMatrix3fv(glGetUniformLocation(m_shader, "m_inverse_transpose"), 1, GL_FALSE, &curr_ITCTM[0][0]);

        /*Pass in m_view and m_proj*/
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);

        glUniform1f(glGetUniformLocation(m_shader, "k_a"), m_metaData.globalData.ka);
        glUniform1f(glGetUniformLocation(m_shader, "k_d"), m_metaData.globalData.kd);

        /*Loop through lights and send to fragment shader*/
        glUniform1i(glGetUniformLocation(m_shader, "total_lights"), m_metaData.lights.size());
        for(int i = 0; i < m_metaData.lights.size(); i++){
            std::string light_dir = "myLights[" + std::to_string(i) + "].dir";
            GLint lights_dir_loc = glGetUniformLocation(m_shader, light_dir.c_str());
            glUniform3f(lights_dir_loc, m_metaData.lights[i].dir[0], m_metaData.lights[i].dir[1], m_metaData.lights[i].dir[2]);

            std::string light_color = "myLights[" + std::to_string(i) + "].color";
            GLint lights_color_loc = glGetUniformLocation(m_shader, light_color.c_str());
            glUniform3f(lights_color_loc, m_metaData.lights[i].color[0], m_metaData.lights[i].color[1], m_metaData.lights[i].color[2]);
        }

        /*Pass shininess, m_ks, and world-space camera position*/
        glUniform1f(glGetUniformLocation(m_shader, "k_s"), m_metaData.globalData.ks);
        auto cam_pos = m_metaData.cameraData.pos;
        //check for shininess == 0 edge case
        auto shiny = shape.primitive.material.shininess == 0 ? 1 : shape.primitive.material.shininess;
        glUniform1f(glGetUniformLocation(m_shader, "shiny"), shiny);
        glUniform4f(glGetUniformLocation(m_shader, "world_cam_pos"), cam_pos[0], cam_pos[1], cam_pos[2], cam_pos[3]);

        /*Send material colors to fragment shader*/
        glUniform3f(glGetUniformLocation(m_shader, "cAmbient"), shape.primitive.material.cAmbient[0], shape.primitive.material.cAmbient[1], shape.primitive.material.cAmbient[2]);
        glUniform3f(glGetUniformLocation(m_shader, "cDiffuse"), shape.primitive.material.cDiffuse[0], shape.primitive.material.cDiffuse[1], shape.primitive.material.cDiffuse[2]);
        glUniform3f(glGetUniformLocation(m_shader, "cSpecular"), shape.primitive.material.cSpecular[0], shape.primitive.material.cSpecular[1], shape.primitive.material.cSpecular[2]);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, shape_data_sizes[size_index] / 6);

        // Unbind Vertex Array
        glBindVertexArray(0);

        //unbind shader
        glUseProgram(0);
    }
}

/**
 * @brief Realtime::resizeGL
 * @param w: screen width
 * @param h: screen height
 * Updates projection matrix if user changes screen size
 */
void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, size().width(), size().height(), m_metaData.cameraData.heightAngle);
}

/**
 * @brief Realtime::sceneChanged
 * Loads in new scene data to m_metaData variable and updates view and projection matrices accordingly
 */
void Realtime::sceneChanged() {
    bool success  = SceneParser::parse(settings.sceneFilePath, m_metaData);
    m_view = Camera::updateViewMat(m_metaData.cameraData);
    m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, width(), height(), m_metaData.cameraData.heightAngle);

    update(); // asks for a PaintGL() call to occur
}

/**
 * @brief Realtime::settingsChanged
 * Checks if a setting of relevance (near/far, shape1/shape2) has occurred and updates accordingly
 */
void Realtime::settingsChanged() {
    /*If GL is initialized*/
    if(done){
        /*Update proj matrix if near or far changes*/
        if(curr_near != settings.nearPlane || curr_far != settings.farPlane){
            m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, size().width(), size().height(), m_metaData.cameraData.heightAngle);
            curr_near = settings.nearPlane;
            curr_far = settings.farPlane;
        }

        /*Update base shapes if tesselation parameters change*/
        if(curr_p1 != settings.shapeParameter1 || curr_p2 != settings.shapeParameter2){
            cube_data.updateParams(settings.shapeParameter1);
            sphere_data.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            cyl_data.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            cone_data.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            curr_p1 = settings.shapeParameter1;
            curr_p2 = settings.shapeParameter2;

            //send changes to vbos
            updateBuffers();
        }

    }

    update(); // asks for a PaintGL() call to occur
}




// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    update(); // asks for a PaintGL() call to occur
}
