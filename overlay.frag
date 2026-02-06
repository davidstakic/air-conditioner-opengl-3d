#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D uTexture;
uniform float uOpacity;

void main() {
    vec4 color = texture(uTexture, TexCoord);
    color.a *= uOpacity;
    FragColor = color;
}
