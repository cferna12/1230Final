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
#include "OBJ-Loader/Source/OBJ_Loader.h"

// ================== Project 5: Lights, Camera


bool done = false;
float curr_x = 0.f;
float curr_y = 0.f;
float theta = 180.f;

bool using_obj_loader = false;
objl::Loader loader;// = new objl::Loader();
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
    m_devicePixelRatio = this->devicePixelRatio();

    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;


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

    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");
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


    /*Fullscreen quad???*/
    std::vector<GLfloat> fullscreen_quad_data =
    { //     POSITIONS    //
      -1.f,  1.f, 0.0f,
      0.0f,  1.f, //uv1

      -1.f, -1.f, 0.0f,
      0.0f, 0.0f, //uv2

      1.f, -1.f, 0.0f,
      1.f, 0.0f, //uv3

      1.f,  1.f, 0.0f,
      1.f,  1.f, //uv4

      -1.f,  1.f, 0.0f,
      0.0f,  1.f, //uv5

      1.f, -1.f, 0.0f,
      1.f, 0.0f //uv6
    };

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*> (3*sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //do i need this clear?

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glViewport(0, 0, m_fbo_width, m_fbo_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint curr_vao_id = 0;
    int size_index = 0;

    glm::mat4 curr_CTM;
    glm::mat3 curr_ITCTM;

    glUseProgram(m_shader);
    if(using_obj_loader){
        glBindVertexArray(obj_vao);
//        auto shiny = shape.primitive.material.shininess == 0 ? 1 : shape.primitive.material.shininess;

        glUniform1i(glGetUniformLocation(m_shader, ""))
        glUniform1f(glGetUniformLocation(m_shader, "shiny"), loader.LoadedMeshes[0].MeshMaterial.Ns);

        /*Send material colors to fragment shader*/
        glUniform3fv(glGetUniformLocation(m_shader, "cAmbient"), 1, &loader.LoadedMeshes[0].MeshMaterial.Ka.X);
        glUniform3fv(glGetUniformLocation(m_shader, "cDiffuse"), 1, &loader.LoadedMeshes[0].MeshMaterial.Kd.X);
        glUniform3fv(glGetUniformLocation(m_shader, "cSpecular"), 1, &loader.LoadedMeshes[0].MeshMaterial.Ks.X);

        glUniform1f(glGetUniformLocation(m_shader, "k_a"), 1.f);
        glUniform1f(glGetUniformLocation(m_shader, "k_d"), 1.f);
        glUniform1f(glGetUniformLocation(m_shader, "k_s"), 1.f);
    }

    else{

    /*Pass in Camera data and global data*/
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_view"), 1, GL_FALSE, &m_view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_proj"), 1, GL_FALSE, &m_proj[0][0]);

    auto cam_pos = m_metaData.cameraData.pos;
    glUniform4fv(glGetUniformLocation(m_shader, "world_cam_pos"), 1, &cam_pos[0]);

    glUniform1f(glGetUniformLocation(m_shader, "k_a"), m_metaData.globalData.ka);
    glUniform1f(glGetUniformLocation(m_shader, "k_d"), m_metaData.globalData.kd);
    glUniform1f(glGetUniformLocation(m_shader, "k_s"), m_metaData.globalData.ks);


    glUniform1i(glGetUniformLocation(m_shader, "toon_shading"), settings.extraCredit3);

    glUniform1i(glGetUniformLocation(m_shader, "rim_thickness"), settings.toon1);
    glUniform1i(glGetUniformLocation(m_shader, "toon_levels"), settings.toon2);
    glUniform1f(glGetUniformLocation(m_shader, "rim_length"), settings.toon3);
//    glUniform1f(glGetUniformLocation(m_shader, "alternate_rim"), settings.extraCredit4);

//    std::cout << settings.toon3 << std::endl;
    /*Loop through lights and send to fragment shader*/
    glUniform1i(glGetUniformLocation(m_shader, "total_lights"), m_metaData.lights.size());
    glm::vec3 light_dir = glm::vec3(0,-1,0);

    bool spaceship = settings.extraCredit4;
    bool moving_light = settings.extraCredit1;
    if(spaceship && settings.extraCredit3){
        glm::vec3 light_dir_test = glm::vec3(0,-1,0);
        std::string light_dir = "myLights[" + std::to_string(0) + "].dir";
        GLint lights_dir_loc = glGetUniformLocation(m_shader, light_dir.c_str());
        glUniform3fv(lights_dir_loc, 1, &light_dir_test[0]);

        std::string light_type = "myLights[" + std::to_string(0) + "].type";
        GLint lights_type_loc = glGetUniformLocation(m_shader, light_type.c_str());
        glUniform1i(lights_type_loc, (int) 1);
    }

    else if(moving_light){
        auto light_dir_test = glm::vec3(-curr_x, -curr_y, 0);

        std::string light_dir = "myLights[" + std::to_string(0) + "].dir";
        GLint lights_dir_loc = glGetUniformLocation(m_shader, light_dir.c_str());
        glUniform3fv(lights_dir_loc, 1, &light_dir_test[0]);
        std::string light_type = "myLights[" + std::to_string(0) + "].type";
        GLint lights_type_loc = glGetUniformLocation(m_shader, light_type.c_str());
        glUniform1i(lights_type_loc, (int) 1);

        std::string light_color = "myLights[" + std::to_string(0) + "].color";
        glm::vec3 light_col = glm::vec3(1);
        GLint lights_color_loc = glGetUniformLocation(m_shader, light_color.c_str());
        glUniform3fv(lights_color_loc, 1,  &light_col[0]);

    }

    else{
    for(int i = 0; i < m_metaData.lights.size(); i++){
        std::string light_dir = "myLights[" + std::to_string(i) + "].dir";
        GLint lights_dir_loc = glGetUniformLocation(m_shader, light_dir.c_str());
        glUniform3fv(lights_dir_loc, 1, &m_metaData.lights[i].dir[0]);

        std::string light_color = "myLights[" + std::to_string(i) + "].color";
        GLint lights_color_loc = glGetUniformLocation(m_shader, light_color.c_str());
        glUniform3fv(lights_color_loc, 1,  &m_metaData.lights[i].color[0]);

        std::string light_pos = "myLights[" + std::to_string(i) + "].pos";
        GLint lights_pos_loc = glGetUniformLocation(m_shader, light_pos.c_str());
        glUniform3fv(lights_pos_loc, 1, &m_metaData.lights[i].pos[0]);

        std::string light_func = "myLights[" + std::to_string(i) + "].function";
        GLint lights_func_loc = glGetUniformLocation(m_shader, light_func.c_str());
        glUniform3fv(lights_func_loc, 1, &m_metaData.lights[i].function[0]);

        std::string light_angle = "myLights[" + std::to_string(i) + "].angle";
        GLint lights_angle_loc = glGetUniformLocation(m_shader, light_angle.c_str());
        glUniform1f(lights_angle_loc, m_metaData.lights[i].angle);

        std::string light_penumbra = "myLights[" + std::to_string(i) + "].penumbra";
        GLint lights_penumbra_loc = glGetUniformLocation(m_shader, light_penumbra.c_str());
        glUniform1f(lights_penumbra_loc, m_metaData.lights[i].penumbra);

        std::string light_type = "myLights[" + std::to_string(i) + "].type";
        GLint lights_type_loc = glGetUniformLocation(m_shader, light_type.c_str());
        glUniform1i(lights_type_loc, (int) m_metaData.lights[i].type);

    }
    }

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

        /*Update current model matrices and VAO*/
        curr_CTM = shape.ctm;
        curr_ITCTM = shape.inv_transpose;
        glBindVertexArray(curr_vao_id);

        /*Send shape matrices*/
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m_model"), 1, GL_FALSE, &curr_CTM[0][0]);
        glUniformMatrix3fv(glGetUniformLocation(m_shader, "m_inverse_transpose"), 1, GL_FALSE, &curr_ITCTM[0][0]);

        //check for shininess == 0 edge case
        auto shiny = shape.primitive.material.shininess == 0 ? 1 : shape.primitive.material.shininess;
        glUniform1f(glGetUniformLocation(m_shader, "shiny"), shiny);

        /*Send material colors to fragment shader*/
        glUniform3fv(glGetUniformLocation(m_shader, "cAmbient"), 1, &shape.primitive.material.cAmbient[0]);
        glUniform3fv(glGetUniformLocation(m_shader, "cDiffuse"), 1, &shape.primitive.material.cDiffuse[0]);
        glUniform3fv(glGetUniformLocation(m_shader, "cSpecular"), 1, &shape.primitive.material.cSpecular[0]);

        // Draw Command
        glDrawArrays(GL_TRIANGLES, 0, shape_data_sizes[size_index] / 6);

        // Unbind Vertex Array
        glBindVertexArray(0);
    }
    }
    glUseProgram(0);



    /*Framebuffer texture*/
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0,0, m_screen_width, m_screen_height);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintTexture(m_fbo_texture);

}

/**
 * @brief Realtime::paintTexture
 * @param texture
 */
void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_texture_shader);

    /*Only send kernels and screen size if kernelbased filter enabled*/
    if(settings.extraCredit2 || settings.kernelBasedFilter){
        glm::mat3 k = {-1.f,-1.f,-1.f,
                      -1.f,17.f,-1.f,
                      -1.f,-1.f,-1.f};


         glUniformMatrix3fv(glGetUniformLocation(m_texture_shader, "k"), 1, GL_FALSE, &k[0][0]);
         glUniform1i(glGetUniformLocation(m_texture_shader, "width"), m_screen_width);
         glUniform1i(glGetUniformLocation(m_texture_shader, "height"), m_screen_height);
    }

    /*Send booleans of selected filters*/
    glUniform1i(glGetUniformLocation(m_texture_shader, "kernel_based"), settings.kernelBasedFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "invert"), settings.perPixelFilter);
    glUniform1i(glGetUniformLocation(m_texture_shader, "grayscale"), settings.extraCredit1);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sharpen"), settings.extraCredit2);


    glBindVertexArray(m_fullscreen_vao);

    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

/**
 * @brief Realtime::makeFBO
 * Makes an FBO with color and depth-stencil attachments
 */
void Realtime::makeFBO(){
    // Task 19: Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    //empty data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    //unind?
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
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

    /*Resize fbo code from lab 11*/
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    /*Resizes FBO data*/
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    makeFBO();

    /*Update proj matrix*/
    m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, size().width(), size().height(), m_metaData.cameraData.heightAngle);
}

/**
 * @brief Realtime::sceneChanged
 * Loads in new scene data to m_metaData variable and updates view and projection matrices accordingly
 */
void Realtime::sceneChanged() {
    bool obj_file = false;
    if(obj_file){

        loader.LoadFile(settings.sceneFilePath);
        glGenBuffers(1, &obj_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, obj_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*loader.LoadedMeshes[0].Vertices.size(), loader.LoadedMeshes[0].Vertices.data(), GL_STATIC_DRAW);

        // Generate, and bind vao
        glGenVertexArrays(1, &obj_vao);
        glBindVertexArray(obj_vao);

        //position
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat),reinterpret_cast<void *>(0));

        // Enable and define attribute 1 to store vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat), reinterpret_cast<void *>(3*sizeof(GLfloat)));

        //texture
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(GLfloat), reinterpret_cast<void *>(6*sizeof(GLfloat)));

        // Clean-up bindings
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);



    }
    else{
    bool success  = SceneParser::parse(settings.sceneFilePath, m_metaData);
    m_view = Camera::updateViewMat(m_metaData.cameraData);
    m_proj = Camera::updateProjMat(settings.nearPlane, settings.farPlane, width(), height(), m_metaData.cameraData.heightAngle);
    update(); // asks for a PaintGL() call to occur
    }
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

/**
 * @brief RodriguesX
 * @param deltaY: Mouse X distance traveled, corresponds to angle of rotation
 * @return Rotation matrix
 */
glm::mat4 RodriguesX(int deltaX){
    float theta = glm::radians((float) deltaX/15.f);
    glm::vec4 col_1 = {cos(theta), 0, -sin(theta), 0};
    glm::vec4 col_2 = {0, cos(theta) + (1-cos(theta)), 0, 0};
    glm::vec4 col_3 = {sin(theta), 0, cos(theta), 0};
    glm::vec4 col_4 = {0,0,0,1};

    auto matrix = glm::mat4(col_1, col_2, col_3, col_4); //should be correct
    return matrix;
}

/**
 * @brief RodriguesY
 * @param deltaY: Mouse Y distance traveled, corresponds to angle of rotation
 * @param axis: Axis to rotate about
 * @return Rotation matrix
 */
glm::mat4 RodriguesY(int deltaY, glm::vec3 axis){
    float theta = glm::radians((float) deltaY/15.f);
    float cos2 = 1- cos(theta);
    float cos1 = cos(theta);
    float sin1 = sin(theta);

    float uxuy_cos2 = axis[0]*axis[1]*cos2;
    float uyuz_cos2 = axis[1]*axis[2]*cos2;
    float uxuz_cos2 = axis[0]*axis[2]*cos2;

    glm::vec4 col_1 = {cos1 + pow(axis[0], 2)*cos2, uxuy_cos2 + axis[2]*sin1, uxuz_cos2 - axis[1]*sin1, 0};
    glm::vec4 col_2 = {uxuy_cos2 - axis[2]*sin1, cos1 + pow(axis[1],2)*cos2, uyuz_cos2 + axis[0]*sin1, 0};
    glm::vec4 col_3 = {uxuz_cos2 + axis[1]*sin1, uyuz_cos2 - axis[0]*sin1, cos1 + pow(axis[2], 2)*cos2, 0};
    glm::vec4 col_4 = {0,0,0,1};

    auto matrix = glm::mat4(col_1, col_2, col_3, col_4); //should be correct
    return matrix;
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        m_metaData.cameraData.look = RodriguesX(deltaX)*m_metaData.cameraData.look;
        m_view = Camera::updateViewMat(m_metaData.cameraData);

        auto axis = glm::normalize(glm::cross(glm::vec3(m_metaData.cameraData.look), glm::vec3(m_metaData.cameraData.up)));
        m_metaData.cameraData.look = RodriguesY(deltaY, axis)*m_metaData.cameraData.look;
        m_view = Camera::updateViewMat(m_metaData.cameraData);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float distance = 5*deltaTime; //should be 5 world space units per second
    glm::mat4 translate = glm::mat4(1);

    curr_x = cos(theta);
    curr_y= sin(theta);
    theta -= 0.01;
    //do i need to normalize anything?

    bool update_view = false;
    /*Move forward*/

     if(m_keyMap[Qt::Key::Key_W]){
        m_metaData.cameraData.pos += glm::normalize(m_metaData.cameraData.look)*distance;
        update_view= true;
//        m_view = Camera::updateViewMat(m_metaData.cameraData);
    }

    /*Back*/
    if(m_keyMap[Qt::Key::Key_S]){
        m_metaData.cameraData.pos -= glm::normalize(m_metaData.cameraData.look)*distance;
        update_view = true;
//        m_view = Camera::updateViewMat(m_metaData.cameraData);
    }

    /*Left*/
    if(m_keyMap[Qt::Key::Key_A]){
        m_metaData.cameraData.pos -= glm::vec4(glm::normalize(glm::cross(glm::vec3(m_metaData.cameraData.look), glm::vec3(m_metaData.cameraData.up)))*distance, 0);
//        m_view = Camera::updateViewMat(m_metaData.cameraData);
        update_view = true;
    }

    /*Right*/
    if(m_keyMap[Qt::Key::Key_D]){
        m_metaData.cameraData.pos += glm::vec4(glm::normalize(glm::cross(glm::vec3(m_metaData.cameraData.look), glm::vec3(m_metaData.cameraData.up)))*distance, 0);
//        m_view = Camera::updateViewMat(m_metaData.cameraData);
        update_view = true;
    }

    /*Move up*/
    if(m_keyMap[Qt::Key::Key_Space]){
        m_metaData.cameraData.pos += glm::vec4(0,1,0,0)*distance;
//        m_view = Camera::updateViewMat(m_metaData.cameraData);
        update_view = true;
    }

    /*Move down*/
    if(m_keyMap[Qt::Key::Key_Control]){
        m_metaData.cameraData.pos += glm::vec4(0,-1,0,0)*distance;
//        m_view = Camera::updateViewMat(m_metaData.cameraData);
        update_view = true;
    }

    if(update_view){
        m_view = Camera::updateViewMat(m_metaData.cameraData);
    }

    update(); // asks for a PaintGL() call to occur
}
