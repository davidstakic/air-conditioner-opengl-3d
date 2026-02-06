#include "HollowCylinder.h"

// Ovi definovi su bitni za matematicke konstante
#define _USE_MATH_DEFINES
#include <cmath>
#include <cstddef> // OBAVEZNO: Ovo resava 'offsetof' gresku

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

HollowCylinder::HollowCylinder(float innerRadius, float outerRadius, float height, int segments)
    : innerR(innerRadius), outerR(outerRadius), h(height), segs(segments)
{
    VAO = VBO = EBO = 0;
    setupMesh();
}

HollowCylinder::~HollowCylinder() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void HollowCylinder::setupMesh()
{
    vertices.clear();
    indices.clear();

    // Da bi svetlo radilo kako treba, moramo razdvojiti povrsine.
    // Ne mozemo koristiti isti verteks za zid i za dno, jer imaju razlicite normale.

    int offset = 0;

    // ---------------------------------------------------------
    // 1. ZIDOVI (Kao u tvom starom kodu, ali sa normalama sa strane)
    // ---------------------------------------------------------
    for (int i = 0; i <= segs; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segs);
        float x = cos(theta);
        float z = sin(theta);
        float u = float(i) / float(segs);

        // -- Spoljasnji zid (Normala ka spolja) --
        glm::vec3 outNorm(x, 0.0f, z);
        vertices.push_back({ glm::vec3(x * outerR, 0.0f, z * outerR), glm::vec2(u, 0.0f), outNorm }); // Dole
        vertices.push_back({ glm::vec3(x * outerR, h,    z * outerR), glm::vec2(u, 1.0f), outNorm }); // Gore

        // -- Unutrasnji zid (Normala ka unutra - ka centru) --
        glm::vec3 inNorm(-x, 0.0f, -z);
        vertices.push_back({ glm::vec3(x * innerR, 0.0f, z * innerR), glm::vec2(u, 0.0f), inNorm }); // Dole
        vertices.push_back({ glm::vec3(x * innerR, h,    z * innerR), glm::vec2(u, 1.0f), inNorm }); // Gore
    }

    // Indeksi za zidove
    for (int i = 0; i < segs; ++i) {
        // Svaki korak 'i' je generisao 4 verteksa u nizu iznad:
        // 0: OutBot, 1: OutTop, 2: InBot, 3: InTop
        int base = i * 4;

        // Spoljasnji zid
        indices.push_back(base + 0); indices.push_back(base + 4); indices.push_back(base + 5);
        indices.push_back(base + 0); indices.push_back(base + 5); indices.push_back(base + 1);

        // Unutrasnji zid
        indices.push_back(base + 2); indices.push_back(base + 7); indices.push_back(base + 6);
        indices.push_back(base + 2); indices.push_back(base + 3); indices.push_back(base + 7);
    }
    offset = vertices.size();

    // ---------------------------------------------------------
    // 2. GORNJI OBOD (Prsten koji spaja zidove gore)
    // Normala gleda u nebo (0, 1, 0)
    // ---------------------------------------------------------
    for (int i = 0; i <= segs; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segs);
        float x = cos(theta);
        float z = sin(theta);
        float u = float(i) / float(segs);

        glm::vec3 upNorm(0.0f, 1.0f, 0.0f);

        vertices.push_back({ glm::vec3(x * innerR, h, z * innerR), glm::vec2(u, 0.0f), upNorm });
        vertices.push_back({ glm::vec3(x * outerR, h, z * outerR), glm::vec2(u, 1.0f), upNorm });
    }

    for (int i = 0; i < segs; ++i) {
        int base = offset + i * 2;
        indices.push_back(base + 0); indices.push_back(base + 3); indices.push_back(base + 1);
        indices.push_back(base + 0); indices.push_back(base + 2); indices.push_back(base + 3);
    }
    offset = vertices.size();

    // ---------------------------------------------------------
    // 3. DNO LAVORA (Od centra do unutrasnjeg zida ili spoljasnjeg)
    // Normala gleda u nebo (0, 1, 0) - da bi dno bilo osvetljeno odozgo
    // ---------------------------------------------------------

    // Centar tacka
    CylVertex center;
    center.Position = glm::vec3(0.0f, 0.0f, 0.0f);
    center.TexCoords = glm::vec2(0.5f, 0.5f);
    center.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
    vertices.push_back(center);
    int centerIdx = offset;

    // Krug oko centra (koristimo outerR kao u tvom starom kodu za dno)
    for (int i = 0; i <= segs; ++i) {
        float theta = 2.0f * M_PI * float(i) / float(segs);
        float x = cos(theta);
        float z = sin(theta);

        // Teksture mapirane planarno (kao krug)
        float u = (x + 1.0f) * 0.5f;
        float v = (z + 1.0f) * 0.5f;

        vertices.push_back({
            glm::vec3(x * outerR, 0.0f, z * outerR), // Pozicija dna
            glm::vec2(u, v),
            glm::vec3(0.0f, 1.0f, 0.0f) // Normala na gore!
            });
    }

    // Trouglovi dna (lepeza)
    for (int i = 0; i < segs; ++i) {
        indices.push_back(centerIdx);
        indices.push_back(centerIdx + 1 + i + 1);
        indices.push_back(centerIdx + 1 + i);
    }

    // ---------------------------------------------------------
    // BAFERI
    // ---------------------------------------------------------
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Pazi: sizeof(CylVertex)
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(CylVertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Layout 0: Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CylVertex), (void*)0);
    glEnableVertexAttribArray(0);

    // Layout 2: TexCoords
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(CylVertex), (void*)offsetof(CylVertex, TexCoords));
    glEnableVertexAttribArray(2);

    // Layout 3: Normal
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(CylVertex), (void*)offsetof(CylVertex, Normal));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void HollowCylinder::draw()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}