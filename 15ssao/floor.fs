#version 150

in vec3 vPosition;
in vec3 vNormal;
in vec3 vViewPosition;
uniform vec3 lightColor;
uniform vec3 lightPosition;
out vec4 outColor;

float checkerboard(float x, float y)
{
    const float size = 0.25;
    const float sharp = 0.1;
    float fx = mod(x / size, 2.0);
    float fy = mod(y / size, 2.0);
    fx = smoothstep(1.0 - sharp, 1.0, fx);
    fy = smoothstep(1.0 - sharp, 1.0, fy);
    return fx * (1.0 - fy) + (1.0 - fx) * fy;
}

void main()
{
    vec3 P = vPosition;
    vec3 N = normalize(vNormal);
    vec3 L = lightPosition - P;
    float r = length(L);
    L /= r;

    float NdotL = max(dot(N, L), 0.0);
    float pattern = checkerboard(vPosition.x, vPosition.z);

    vec3 color = vec3(0.3, 0.4, 0.6);
    color = color * pattern + vec3(0.1, 0.12, 0.15) * color * (1.0 - pattern);
    color *= NdotL * lightColor;

    // float dist2 = dot(vViewPosition, vViewPosition);
    // float fog = exp(-0.1 * dist2);
    // color *= fog;

    outColor.rgb = color;
    outColor.a = 1.0;
}