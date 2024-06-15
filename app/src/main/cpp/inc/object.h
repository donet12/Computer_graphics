#ifndef HW3_OBJECT_H
#define HW3_OBJECT_H

#include "global.h"
#include "program.h"
#include "material.h"
#include "vertex.h"

class Object {
public:
    Object(Program* program, Material* material,
           const vector<Vertex>& vertices, const vector<Index>& indices, GLenum primitive=GL_TRIANGLES);
    Object(Program* program, Material* material,
            const vector<Vertex>& vertices, const vector<Index>& indices, const vector<vec3>& tangents, GLenum primitive=GL_TRIANGLES);
    virtual ~Object();

    virtual void draw() const;
    virtual void calculateTangents(vector<vec3> &tangentArr);

    mat4 worldMat;
    Material* material;
    vector<Vertex> vertices;
    vector<Index> indices;
    vector<vec3> tangents;
    virtual void load(const vector<Vertex>& vertices, const vector<Index>& indices);
    virtual void load(const vector<Vertex>& vertices, const vector<Index>& indices, const vector<vec3>& tangents);

private:
    Program* program;
    GLuint vbo, ibo, tbo;
    GLenum primitive;
    virtual void create(Program* program, Material* material);
    virtual void destroy();
};

#endif //HW3_OBJECT_H
