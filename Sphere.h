#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Sphere {
public:
    Sphere(float radius, int slices = 16, int stacks = 16);
    ~Sphere();

    void setPosition(const glm::vec3& pos) { position = pos; }
    glm::vec3 getPosition() const { return position; }

    void draw();

private:
    GLuint VAO, VBO, EBO;
    int vertexCount;
    glm::vec3 position;
};
