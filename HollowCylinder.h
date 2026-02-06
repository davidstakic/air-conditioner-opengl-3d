#pragma once
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

// Preimenovali smo strukturu da ne pravi konflikt sa tvojim model.hpp/mesh.hpp
struct CylVertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

class HollowCylinder {
public:
    HollowCylinder(float innerRadius, float outerRadius, float height, int segments);
    ~HollowCylinder();

    void draw();

private:
    float innerR, outerR, h;
    int segs;
    GLuint VAO, VBO, EBO;

    // Koristimo nasu unikatnu strukturu
    std::vector<CylVertex> vertices;
    std::vector<unsigned int> indices;

    void setupMesh();
};