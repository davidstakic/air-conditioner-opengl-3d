#version 330 core

layout(location = 0) in vec3 inPos;
layout(location = 2) in vec2 inTex;
layout(location = 3) in vec3 inNormal;

uniform mat4 uM;
uniform mat4 uV;
uniform mat4 uP;

out vec3 FragPos;
out vec3 Normal;
out vec2 channelTex;

uniform bool useUniformColor;

void main()
{
    FragPos = vec3(uM * vec4(inPos, 1.0));
    Normal = mat3(transpose(inverse(uM))) * inNormal;

    gl_Position = uP * uV * vec4(FragPos, 1.0);

    channelTex = inTex;
}
