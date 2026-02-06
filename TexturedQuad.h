#pragma once
#include <GL/glew.h>

class TexturedQuad {
public:
    TexturedQuad(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height)
    {
        float vertices[] = {
            x, y, 0.0f, 0.0f,
            x, y + height, 0.0f, 1.0f,
            x + width, y + height, 1.0f, 1.0f,
            x + width, y, 1.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
    }

    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

private:
    GLuint VAO, VBO;
    float x, y, width, height;
};
