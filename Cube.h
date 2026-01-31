#pragma once
#include <GL/glew.h>

class Cube {
public:
    Cube(float w, float h, float d);
    ~Cube();

    void draw();

private:
    unsigned int VAO;
    unsigned int VBO;
};
