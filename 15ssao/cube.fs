#version 150

in vec3 vPosition;
in vec3 vNormal;
uniform vec3 lightColor;
uniform vec3 lightPosition;
out vec4 outColor;

void main()
{
    vec3 P = vPosition;
    vec3 N = normalize(vNormal);
    vec3 L = lightPosition - P;
    float r = length(L);
    L /= r;

    float NdotL = max(dot(N, L), 0.0);
    outColor.rgb = NdotL * vec3(1.0, 0.4, 0.3) * lightColor;
    outColor.a = 1.0;
}