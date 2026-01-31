#include "Cube.h"

Cube::Cube(float w, float h, float d)
{
    float vertices[] =
    {
        -w, -h,  d,   0.9, 0.9, 0.9, 1.0,   0,0,    0,0,1,
         w, -h,  d,   0.9, 0.9, 0.9, 1.0,   1,0,    0,0,1,
         w,  h,  d,   0.9, 0.9, 0.9, 1.0,   1,1,    0,0,1,
        -w,  h,  d,   0.9, 0.9, 0.9, 1.0,   0,1,    0,0,1,

        -w, -h, -d,   0.8, 0.8, 0.8, 1.0,   0,0,    0,0,-1,
        -w,  h, -d,   0.8, 0.8, 0.8, 1.0,   0,1,    0,0,-1,
         w,  h, -d,   0.8, 0.8, 0.8, 1.0,   1,1,    0,0,-1,
         w, -h, -d,   0.8, 0.8, 0.8, 1.0,   1,0,    0,0,-1,

         -w, -h, -d,   0.7, 0.7, 0.7, 1.0,   0,0,   -1,0,0,
         -w, -h,  d,   0.7, 0.7, 0.7, 1.0,   1,0,   -1,0,0,
         -w,  h,  d,   0.7, 0.7, 0.7, 1.0,   1,1,   -1,0,0,
         -w,  h, -d,   0.7, 0.7, 0.7, 1.0,   0,1,   -1,0,0,

          w, -h, -d,   0.7, 0.7, 0.7, 1.0,   0,0,    1,0,0,
          w,  h, -d,   0.7, 0.7, 0.7, 1.0,   0,1,    1,0,0,
          w,  h,  d,   0.7, 0.7, 0.7, 1.0,   1,1,    1,0,0,
          w, -h,  d,   0.7, 0.7, 0.7, 1.0,   1,0,    1,0,0,

          -w,  h,  d,   1.0, 1.0, 1.0, 1.0,   0,0,    0,1,0,
           w,  h,  d,   1.0, 1.0, 1.0, 1.0,   1,0,    0,1,0,
           w,  h, -d,   1.0, 1.0, 1.0, 1.0,   1,1,    0,1,0,
          -w,  h, -d,   1.0, 1.0, 1.0, 1.0,   0,1,    0,1,0,

          -w, -h,  d,   0.6, 0.6, 0.6, 1.0,   0,0,    0,-1,0,
          -w, -h, -d,   0.6, 0.6, 0.6, 1.0,   0,1,    0,-1,0,
           w, -h, -d,   0.6, 0.6, 0.6, 1.0,   1,1,    0,-1,0,
           w, -h,  d,   0.6, 0.6, 0.6, 1.0,   1,0,    0,-1,0,
    };

    unsigned int stride = (3 + 4 + 2 + 3) * sizeof(float);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(10 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

Cube::~Cube()
{
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Cube::draw()
{
    glBindVertexArray(VAO);
    for (int i = 0; i < 6; i++)
        glDrawArrays(GL_TRIANGLE_FAN, i * 4, 4);
    glBindVertexArray(0);
}
