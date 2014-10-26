#version 150

in vec2 uv;
in vec3 up;
in vec3 right;
in vec3 forward;
out vec4 outColor;

float floor(vec3 ro, vec3 rd, float h)
{
    return (h - ro.y) / rd.y;
}

void main()
{
    const float focalLength = 1.2;
    const vec3 skyColor = vec3(0.05, 0.08, 0.1);
    vec3 ro = vec3(0.0);
    vec3 rd = normalize(forward * focalLength + uv.x * right + uv.y * up);
    float t = floor(ro, rd, -1.0);

    vec3 color = skyColor;
    if (t >= 0.0)
    {
        color = vec3(1.0, 0.3, 0.2);
        float fog = exp(-0.1 * sqrt(t));
        color = color * fog + skyColor * (1.0 - fog);
    }


    outColor = vec4(color, 1.0);
}