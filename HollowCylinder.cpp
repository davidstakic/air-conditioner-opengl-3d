#include "HollowCylinder.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

HollowCylinder::HollowCylinder(float innerRadius, float outerRadius, float height, int segments)
    : innerR(innerRadius), outerR(outerRadius), h(height), segs(segments)
{
    VAO = VBO = EBO = 0;

    setupMesh();
}

void HollowCylinder::setupMesh()
{
    vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));

    for (int i = 0; i <= segs; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segs);
        float x = cos(theta);
        float z = sin(theta);

        vertices.push_back(glm::vec3(innerR * x, 0.0f, innerR * z));

        vertices.push_back(glm::vec3(innerR * x, h, innerR * z));

        vertices.push_back(glm::vec3(outerR * x, h, outerR * z));

        vertices.push_back(glm::vec3(outerR * x, 0.0f, outerR * z));
    }

    for (int i = 0; i < segs; ++i) {
        int baseCurrent = 1 + i * 4;
        int baseNext = 1 + (i + 1) * 4;

        int inBotCur = baseCurrent;
        int inBotNext = baseNext;

        int inTopCur = baseCurrent + 1;
        int inTopNext = baseNext + 1;

        int outTopCur = baseCurrent + 2;
        int outTopNext = baseNext + 2;

        int outBotCur = baseCurrent + 3;
        int outBotNext = baseNext + 3;

        indices.push_back(0);
        indices.push_back(inBotNext);
        indices.push_back(inBotCur);

        indices.push_back(inBotCur);
        indices.push_back(inBotNext);
        indices.push_back(inTopNext);

        indices.push_back(inBotCur);
        indices.push_back(inTopNext);
        indices.push_back(inTopCur);

        indices.push_back(inTopCur);
        indices.push_back(inTopNext);
        indices.push_back(outTopNext);

        indices.push_back(inTopCur);
        indices.push_back(outTopNext);
        indices.push_back(outTopCur);

        indices.push_back(outTopCur);
        indices.push_back(outTopNext);
        indices.push_back(outBotNext);

        indices.push_back(outTopCur);
        indices.push_back(outBotNext);
        indices.push_back(outBotCur);

        indices.push_back(0);
        indices.push_back(outBotCur);
        indices.push_back(outBotNext);
    }


    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void HollowCylinder::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}