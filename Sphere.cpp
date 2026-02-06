#include "Sphere.h"
#include <vector>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Sphere::Sphere(float radius, int slices, int stacks)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (int i = 0; i <= stacks; ++i) {
        float V = i / (float)stacks;
        float phi = V * M_PI;

        for (int j = 0; j <= slices; ++j) {
            float U = j / (float)slices;
            float theta = U * (M_PI * 2);

            float nx = cos(theta) * sin(phi);
            float ny = cos(phi);
            float nz = sin(theta) * sin(phi);

            vertices.push_back(nx * radius);
            vertices.push_back(ny * radius);
            vertices.push_back(nz * radius);

            vertices.push_back(U);
            vertices.push_back(V);

            vertices.push_back(nx);
            vertices.push_back(ny);
            vertices.push_back(nz);
        }
    }

    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            int first = i * (slices + 1) + j;
            int second = first + slices + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    vertexCount = (unsigned int)indices.size();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    unsigned int stride = 8 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

Sphere::~Sphere()
{
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
}

void Sphere::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}