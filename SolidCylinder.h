#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class SolidCylinder {
public:
    SolidCylinder(float radius, float height, int segments = 32);
    ~SolidCylinder();

    void setHeight(float newHeight);
    void draw();

private:
    void setupMesh();

    float r, h;
    int segs;

    GLuint VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
};
