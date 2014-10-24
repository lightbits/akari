#version 150

in vec2 texel;
in vec2 position;
uniform ivec2 tilesize;
uniform ivec2 visible_res;
uniform ivec2 texsize;
uniform int stride;
uniform isampler2D tilemap;
out vec2 global_texel;
out vec2 vtexel;

void main()
{
    int i = texture(tilemap, texel).r;
    vec2 global_coord = vec2(i % stride, i / stride) * tilesize;
    vec2 local_coord = (0.5 * position + vec2(0.5) - texel) * visible_res * tilesize;
    local_coord.y = tilesize.y - local_coord.y;
    global_texel = global_coord / texsize;
    vtexel = (global_coord + local_coord) / texsize;
    gl_Position = vec4(position, 0.0, 1.0);
}