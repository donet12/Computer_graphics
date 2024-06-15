#include "camera.h"

Camera::Camera(Program* program)
    : eye(vec3(0.0f)), at(vec3(0.0f)), up(vec3(0.0f, 1.0f, 0.0f)),
    fovy(60.0f), aspect(1.0f), zNear(1.0f), zFar(400.0f) {

    this->create(program);
}

void Camera::create(Program* program) {
    LOG_PRINT_DEBUG("Create camera");

    this->program = program;

    if(glGetUniformLocation(program->get(), "viewMat") < 0)
        LOG_PRINT_WARN("Fail to get uniform location: %s", "viewMat");
    if(glGetUniformLocation(program->get(), "projMat") < 0)
        LOG_PRINT_WARN("Fail to get uniform location: %s", "projMat");
    if (glGetUniformLocation(program->get(), "eyePos") < 0);
        LOG_PRINT_ERROR("Fail to get uniform location: %s", "eyePos");

}

void Camera::update() const {
    vec3 eyepose = this->eye;
    mat4 viewMat = lookAt(eye, at, up);
    mat4 projMat = perspective(radians(fovy), aspect, zNear, zFar);


    GLint viewMatLoc = glGetUniformLocation(program->get(), "viewMat");
    GLint projMatLoc = glGetUniformLocation(program->get(), "projMat");
    GLint eyeVecLoc = glGetUniformLocation(program->get(), "eyePos");

    if (viewMatLoc >= 0) glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, value_ptr(viewMat));
    else LOG_PRINT_ERROR("Fail to get uniform location: %s", "viewMat");
    if (projMatLoc >= 0) glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, value_ptr(projMat));
    else LOG_PRINT_ERROR("Fail to get uniform location: %s", "projMat");
    if (eyeVecLoc >= 0) glUniform3fv(eyeVecLoc, 1, value_ptr(vec3(eyepose)));
    else LOG_PRINT_ERROR("Fail to get uniform location: %s", "eyePos");
}