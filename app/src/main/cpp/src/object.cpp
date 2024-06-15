#include "object.h"

Object::Object(Program* program, Material* material,
               const vector<Vertex> &vertices, const vector<Index> &indices,
               GLenum primitive) : primitive(primitive) {

    create(program, material);
    load(vertices, indices);
}

Object::Object(Program* program, Material* material,
        const vector<Vertex> &vertices, const vector<Index> &indices, const vector<vec3> &tangents,
               GLenum primitive) : primitive(primitive) {

    create(program, material);
    load(vertices, indices, tangents);
}

Object::~Object() {
    destroy();
}

void Object::create(Program* program, Material* material) {
    this->program = program;
    this->material = material;

    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    glGenBuffers(1, &tbo);

    if(vbo == 0 || ibo == 0)
        LOG_PRINT_ERROR("Fail to generate buffers");

    if (glGetUniformLocation(program->get(), "worldMat") < 0)
        LOG_PRINT_WARN("Fail to get uniform location: %s", "worldMat");
}
void Object::load(const vector<Vertex> &vertices, const vector<Index> &indices)
{
    //LOG_PRINT_DEBUG("Load object data");

    this->vertices = vertices;
    this->indices = indices;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(),
                 this->vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * this->indices.size(),
                 this->indices.data(), GL_DYNAMIC_DRAW);

}
void Object::load(const vector<Vertex> &vertices, const vector<Index> &indices, const vector<vec3> &tangents)
{
    //LOG_PRINT_DEBUG("Load object data");

    this->vertices = vertices;
    this->indices = indices;
    this->tangents = tangents;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * this->vertices.size(),
            this->vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index) * this->indices.size(),
                 this->indices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * this->tangents.size(),
                 this->tangents.data(), GL_DYNAMIC_DRAW);
}

void Object::draw() const {
    if (material)
        material->update();

    GLint loc = glGetUniformLocation(program->get(), "worldMat");
    if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(worldMat));

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (const void *) offsetof(Vertex, pos));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (const void *) offsetof(Vertex, nor));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (const void *) offsetof(Vertex, tex));

    if (!this->tangents.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, tbo);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
                              sizeof(vec3), (const void *) 0);
    }

    // draw elements
    glDrawElements(primitive, (GLsizei) indices.size(), GL_UNSIGNED_SHORT, (const void *) 0);
}

void Object::calculateTangents(vector<vec3> &tangentArr)
{
    int id0, id1, id2;
    float s10, s20, t10, t20;
    vec3 nor, q1, q2;
    for (int i = 0; i < indices.size()/3; i ++)
    {
        id0 = indices[i*3];
        id1 = indices[i*3 + 1];
        id2 = indices[i*3 + 2];

        s10 = vertices[id1].tex.x - vertices[id0].tex.x;
        s20 = vertices[id2].tex.x - vertices[id0].tex.x;

        t10 = vertices[id1].tex.y - vertices[id0].tex.y;
        t20 = vertices[id2].tex.y - vertices[id0].tex.y;

        q1 = vertices[id1].pos - vertices[id0].pos;
        q2 = vertices[id2].pos - vertices[id0].pos;

        tangentArr[id0] += (t20 * q1 - t10 * q2) / (s10 * t20 - s20 * t10);
        tangentArr[id1] += (t20 * q1 - t10 * q2) / (s10 * t20 - s20 * t10);
        tangentArr[id2] += (t20 * q1 - t10 * q2) / (s10 * t20 - s20 * t10);
    }

    for(int i = 0; i < vertices.size(); i++)
    {
        tangentArr[i] = tangentArr[i] - vertices[i].nor * dot(vertices[i].nor, tangentArr[i]);
        tangentArr[i] = normalize(tangentArr[i]);
    }

    this->tangents = tangentArr;
}

void Object::destroy() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteBuffers(1, &tbo);
}

