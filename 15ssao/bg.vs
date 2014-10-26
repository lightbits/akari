#version 150

in vec2 position;
uniform mat4 invView;
out vec2 uv;
out vec3 up;
out vec3 right;
out vec3 forward;

void main()
{
    gl_Position = vec4(position, 0.0, 1.0);
    up      = (invView * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
    right   = (invView * vec4(1.0, 0.0, 0.0, 0.0)).xyz;
    forward = (invView * vec4(0.0, 0.0, -1.0, 0.0)).xyz;
    uv = position;
}