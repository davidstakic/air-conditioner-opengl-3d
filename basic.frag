#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec4 channelCol;
in vec2 channelTex;

out vec4 outCol;

uniform sampler2D uTex;
uniform bool useTex;
uniform bool transparent;
uniform vec4 uColor;
uniform bool useUniformColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightStrength;

uniform bool lampOn;
uniform vec3 lampPos;
uniform vec3 lampColor;
uniform float lampStrength;

uniform vec3 viewPos;

void main()
{
    vec3 baseColor;

    if (useTex)
        baseColor = texture(uTex, channelTex).rgb;
    else
        baseColor = useUniformColor ? uColor.rgb : vec3(1.0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 lightDir = normalize(lightPos - FragPos);

    vec3 ambient = 0.2 * lightColor;

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.4 * spec * lightColor;

    vec3 globalLight =
        (ambient + diffuse + specular) * lightStrength;

    vec3 lampLight = vec3(0.0);

    if (lampOn)
    {
        vec3 lampDir = normalize(lampPos - FragPos);
        float lampDiff = max(dot(norm, lampDir), 0.0);

        vec3 lampAmbient = 0.05 * lampColor;
        vec3 lampDiffuse = lampDiff * lampColor;

        lampLight = (lampAmbient + lampDiffuse) * lampStrength;
    }

    vec3 result = (globalLight + lampLight) * baseColor;
    outCol = vec4(result, 1.0);
}
