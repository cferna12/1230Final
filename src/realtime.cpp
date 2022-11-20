#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
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
    : QOpenGLWidget(parent),
      m_lightPos(10,0,0,1),
      m_ka(0.1),
      m_kd(0.8),
      m_ks(1),
      m_shininess(15)
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

    //make sure correct paths
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");


    //initial settings
    int p1 = settings.shapeParameter1;
    int p2 = settings.shapeParameter2;

    cube_data.updateParams(p1);
    sphere_data.updateParams(p1, p2);
    cyl_data.updateParams(p1, p2);
    cone_data.updateParams(p1, p2);

//    GLuint m_vbo;
    VBOS.reserve(4);
    VAOS.reserve(4);


    //below is copied from lab10 to bidn for sphere
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    // Generate sphere data
    Sphere s = Sphere();
    s.updateParams(p1,p2);

    m_sphereData = s.generateShape();
    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat), m_sphereData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(GLfloat),reinterpret_cast<void *>(0));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);


//    std::vector<std::vector<float>> primitives = {cube_data.generateShape(), sphere_data.generateShape(), cyl_data.generateShape(), cone_data.generateShape()};
//    //loop through shapes and create/bind necessary vbos and vaos
//    for(int i = 0; i < 4; i++){
//        glGenBuffers(1, &VBOS[i]);
//        glBindBuffer(GL_ARRAY_BUFFER, VBOS[i]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*primitives[i].size(), primitives[i].data(), GL_STATIC_DRAW);

//        // Generate, and bind vao
//        glGenVertexArrays(1, &VAOS[i]);
//        glBindVertexArray(VAOS[i]);

//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(GLfloat),reinterpret_cast<void *>(0));

//        //need attribute for normals??

//        // Clean-up bindings
//        glBindVertexArray(0);
//        glBindBuffer(GL_ARRAY_BUFFER,0);
//    }
    done = true;

}

void Realtime::updateBuffers(){

    //does this need to be a pointer?
    std::vector<std::vector<float>> primitives = {cube_data.generateShape(), sphere_data.generateShape(), cyl_data.generateShape(), cone_data.generateShape()};

    //loop through shapes and create/bind necessary vbos and vaos
//    for(int i = 0; i < 4; i++){
//        glBindBuffer(GL_ARRAY_BUFFER, VBOS[i]);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*primitives[i].size(), primitives[i].data(), GL_STATIC_DRAW);
//        glBindBuffer(GL_ARRAY_BUFFER,0);

//    }
    m_sphereData = sphere_data.generateShape();
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*m_sphereData.size(), m_sphereData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER,0);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Bind Sphere Vertex Data
    glBindVertexArray(m_sphere_vao);

    // Task 2: activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);

    // Task 6: pass in m_model as a uniform into the shader program
    GLint loc = glGetUniformLocation(m_shader, "m_model");
    m_model = glm::mat4(1);

    glUniformMatrix4fv(loc, 1, GL_FALSE, &m_model[0][0]);

    // Task 7: pass in m_view and m_proj
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);

    // Task 12: pass m_ka into the fragment shader as a uniform
    glUniform1f(glGetUniformLocation(m_shader, "k_a"), m_ka);

    // Task 13: pass light position and m_kd into the fragment shader as a uniform
    glUniform1f(glGetUniformLocation(m_shader, "k_d"), m_kd);
    glm::vec4 light_pos = m_lightPos;
    glUniform4f(glGetUniformLocation(m_shader, "world_light_pos"), m_lightPos[0], m_lightPos[1], m_lightPos[2], m_lightPos[3]);

    // Task 14: pass shininess, m_ks, and world-space camera position
    glUniform1f(glGetUniformLocation(m_shader, "k_s"), m_ks);

    glm::vec4 cam_pos = glm::inverse(m_view)*glm::vec4(0,0,0,1);
    glUniform1f(glGetUniformLocation(m_shader, "shiny"), m_shininess);
    glUniform4f(glGetUniformLocation(m_shader, "world_cam_pos"), cam_pos[0], cam_pos[1], cam_pos[2], cam_pos[3]);

    // Draw Command
    glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6);
    // Unbind Vertex Array
    glBindVertexArray(0);

    // Task 3: deactivate the shader program by passing 0 into glUseProgram
    glUseProgram(0);


}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here

    /*Update projection matrix*/
//    m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, size().width(), size().height(), m_metaData.cameraData.heightAngle);
}




void Realtime::sceneChanged() {
    std::cout << "scene changed" << std::endl;

    bool success  = SceneParser::parse(settings.sceneFilePath, m_metaData);

    /*Update camera matrices with new data*/

    //double check with raytracer to see if view matrix correct
//    m_view = Camera::updateViewMat(m_metaData.cameraData);
    m_ka =m_metaData.globalData.ka;
    m_kd =m_metaData.globalData.kd;
      m_ks =m_metaData.globalData.ks;

//       m_ka =m_metaData.globalData.k
    //check with glm::perspective call
//      m_lightPos = m_metaData.lights[0].pos;
//      m_camPos = m_metaData.cameraData.pos;

    float aspect = ((float) width())/((float)height());
//    m_proj = glm::perspective(glm::radians(m_metaData.cameraData.heightAngle), aspect, settings.nearPlane, settings.farPlane);
//    m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, size().width(), size().height(), m_metaData.cameraData.heightAngle);


    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
    /*Update projection matrix if near or far changes*/
    if(done){
        if(curr_near != settings.nearPlane || curr_far != settings.farPlane){
            m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, size().width(), size().height(), m_metaData.cameraData.heightAngle);
            curr_near = settings.nearPlane;
            curr_far = settings.farPlane;
        }

        //update primitives if params change
        if(curr_p1 != settings.shapeParameter1 || curr_p2 != settings.shapeParameter2){
            cube_data.updateParams(settings.shapeParameter1);
            sphere_data.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            cyl_data.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            cone_data.updateParams(settings.shapeParameter1, settings.shapeParameter2);
            curr_p1 = settings.shapeParameter1;
            curr_p2 = settings.shapeParameter2;
            updateBuffers();
        }

    }


//    makeCurrent()
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
