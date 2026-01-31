#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class HollowCylinder {
public:
    HollowCylinder(float innerRadius, float outerRadius, float height, int segments);

    void draw();

private:
    float innerR, outerR, h;
    int segs;
    GLuint VAO, VBO, EBO;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    void setupMesh();
};
