#version 330 core

in vec4 channelCol;
in vec2 channelTex;

out vec4 outCol;

uniform sampler2D uTex;
uniform bool useTex;
uniform bool transparent;
uniform vec4 uColor; // uniform za lampicu
uniform bool useUniformColor; // da li da koristimo uniform za boju

void main()
{
    if(useTex)
    {
        outCol = texture(uTex, channelTex);
        if(!transparent && outCol.a < 1)
            outCol = vec4(1.0,1.0,1.0,1.0);
    }
    else
    {
        if(useUniformColor)
            outCol = uColor;
        else
            outCol = channelCol;
    }
}
