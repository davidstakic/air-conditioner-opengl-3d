#include "SolidCylinder.h"
#include <cmath>

SolidCylinder::SolidCylinder(float radius, float height, int segments)
    : r(radius), h(height), segs(segments)
{
    setupMesh();
}

SolidCylinder::~SolidCylinder() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void SolidCylinder::setHeight(float newHeight) {
    h = newHeight;
    setupMesh();
}

void SolidCylinder::setupMesh() {
    vertices.clear();
    indices.clear();

    for (int i = 0; i < segs; i++) {
        float a = (float)i / segs * 2.0f * 3.1415926f;
        float x = r * cos(a);
        float z = r * sin(a);

        vertices.push_back({ x, 0.0f, z });
        vertices.push_back({ x, h, z });
    }

    for (int i = 0; i < segs; i++) {
        int next = (i + 1) % segs;

        indices.push_back(i * 2);
        indices.push_back(next * 2);
        indices.push_back(i * 2 + 1);

        indices.push_back(i * 2 + 1);
        indices.push_back(next * 2);
        indices.push_back(next * 2 + 1);
    }

    int center = vertices.size();
    vertices.push_back({ 0,0,0 });

    for (int i = 0; i < segs; i++) {
        int next = (i + 1) % segs;
        indices.push_back(center);
        indices.push_back(i * 2);
        indices.push_back(next * 2);
    }

    if (!VAO) {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
        vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
        indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void SolidCylinder::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
