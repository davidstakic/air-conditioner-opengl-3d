#include "SolidCylinder.h"
#include <cmath>
#include <vector>

struct CylinderVertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
};

SolidCylinder::SolidCylinder(float radius, float height, int segments)
    : r(radius), h(height), segs(segments), VAO(0), VBO(0), EBO(0)
{
    setupMesh();
}

SolidCylinder::~SolidCylinder() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void SolidCylinder::setHeight(float newHeight) {
    h = newHeight;
    setupMesh();
}

void SolidCylinder::setupMesh() {
    std::vector<CylinderVertex> meshVertices;
    indices.clear();

    // --- 1. BOČNE STRANICE (Omotač) ---
    for (int i = 0; i <= segs; i++) {
        float a = (float)i / segs * 2.0f * 3.1415926f;
        float nx = cos(a);
        float nz = sin(a);
        float x = r * nx;
        float z = r * nz;
        float u = (float)i / segs;

        // Donja tačka omotača (Normala ide u stranu)
        meshVertices.push_back({ {x, 0.0f, z}, {u, 0.0f}, {nx, 0.0f, nz} });
        // Gornja tačka omotača (Normala ide u stranu)
        meshVertices.push_back({ {x, h, z}, {u, 1.0f}, {nx, 0.0f, nz} });
    }

    for (int i = 0; i < segs; i++) {
        int curr = i * 2;
        int next = (i + 1) * 2;
        indices.push_back(curr); indices.push_back(next); indices.push_back(curr + 1);
        indices.push_back(curr + 1); indices.push_back(next); indices.push_back(next + 1);
    }

    // --- 2. GORNJI POKLOPAC (Površina vode - Normala ide GORE) ---
    int topCapStart = meshVertices.size();
    // Centar gornjeg kruga
    meshVertices.push_back({ {0.0f, h, 0.0f}, {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} });

    for (int i = 0; i <= segs; i++) {
        float a = (float)i / segs * 2.0f * 3.1415926f;
        float x = r * cos(a);
        float z = r * sin(a);
        float u = cos(a) * 0.5f + 0.5f;
        float v = sin(a) * 0.5f + 0.5f;
        meshVertices.push_back({ {x, h, z}, {u, v}, {0.0f, 1.0f, 0.0f} });
    }

    for (int i = 0; i < segs; i++) {
        indices.push_back(topCapStart);
        indices.push_back(topCapStart + 1 + i);
        indices.push_back(topCapStart + 1 + (i + 1));
    }

    // --- OpenGL BUFFERI ---
    if (VAO == 0) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(CylinderVertex), meshVertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    unsigned int stride = sizeof(CylinderVertex);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(CylinderVertex, TexCoords));
    glEnableVertexAttribArray(2);
    // Normals
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(CylinderVertex, Normal));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void SolidCylinder::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}