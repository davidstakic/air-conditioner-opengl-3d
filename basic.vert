#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec4 inCol;
layout(location = 2) in vec2 inTex;

uniform mat4 uM; // Model
uniform mat4 uV; // View
uniform mat4 uP; // Projection

out vec4 channelCol;
out vec2 channelTex;

uniform bool useUniformColor; // nova uniform

void main()
{
    gl_Position = uP * uV * uM * vec4(inPos, 1.0);
    if(useUniformColor)
        channelCol = vec4(0.0); // placeholder, shader ce koristiti uniform
    else
        channelCol = inCol;

    channelTex = inTex;
}
