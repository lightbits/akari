#version 150

in vec2 vtexel;
in vec2 global_texel;
uniform ivec2 texsize;
uniform ivec2 tilesize;
uniform sampler2D tex;
out vec4 fcolor;

void main()
{
    vec2 local_texel = mod(gl_FragCoord.xy, vec2(tilesize)) / vec2(tilesize);
    vec2 uv = local_texel + global_texel;
    fcolor = texture(tex, uv) * 0.0001;
    fcolor += vec4(local_texel, 0.0, 1.0);
    // fcolor = texture(tex, vtexel);
    // fcolor = vec4(vtexel.x, 0.0, 0.0, 0.0);
}

// void main(void)
// {
//     float zoom = 4.0 + sin(iGlobalTime);
//     vec2 tc = mod(gl_FragCoord.xy, zoom * vec2(16.0, 16.0)); // Tile coordinates
//     vec2 uv = tc / vec2(zoom * 16.0);
//     gl_FragColor = vec4(uv, 0.0, 1.0);
// }