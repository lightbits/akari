#include "game.h"
#define GLSL(src) "#version 150 core\n" #src

/*
http://www.slideshare.net/blindrenderer/screen-space-decals-in-warhammer-40000-space-marine-14699854
*/

const char *GEOMETRY_VS = GLSL(
    in vec3 position;
    in vec3 normal;
    out vec3 v_normal;
    out vec3 v_position;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        v_normal = (view * model * vec4(normal, 0.0)).xyz;
        v_position = (view * model * vec4(position, 1.0)).xyz;
        gl_Position = projection * vec4(v_position, 1.0);
    }
);

const char *GEOMETRY_FS = GLSL(
    in vec3 v_position;
    in vec3 v_normal;
    uniform float z_near;
    uniform float z_far;
    out vec4 f_color;

    void main()
    {
        vec3 N = vec3(0.5) + 0.5 * normalize(v_normal);

        // This is the view-space z coordinate mapped from [-near, -far] to [0, 1]
        float zvn = clamp((-v_position.z - z_near) / (z_far - z_near), 0.0, 1.0);
        f_color = vec4(N, zvn);
    }
);

const char *DECAL_VS = GLSL
(
    in vec3 position;
    out vec3 v_box_normal;
    uniform mat4 projection;
    uniform mat4 view;
    uniform mat4 model;
    void main()
    {
        v_box_normal = (view * model * vec4(0.0, 1.0, 0.0, 0.0)).xyz;
        gl_Position = projection * view * model * vec4(position, 1.0);
    }
);

const char *DECAL_FS = GLSL
(
    in vec3 v_box_normal;
    uniform sampler2D tex_geometry;
    uniform sampler2D tex_decal;
    uniform vec2 resolution;
    uniform mat4 view;
    uniform mat4 model;
    uniform float z_near;
    uniform float z_far;
    uniform float tan_fovy_h;
    out vec4 f_color;

    void main()
    {
        vec2 texel = gl_FragCoord.xy / resolution;
        vec4 geometry = texture(tex_geometry, texel);
        float zvn = geometry.w;

        vec3 bN = normalize(v_box_normal);
        vec3 gN = vec3(-1.0) + 2.0 * geometry.xyz;

		// If the angle between the box normal and the
		// geometry normal is too large, the texture will
		// appear janky and stretched across the surface.
		// So we don't render if the angle is too great.
        float cos_angle_between_normals = dot(bN, gN);
        float cos_angle_threshold = 0.5; // cos (60 degrees)

        f_color = vec4(0.0);
        if (zvn <= 0.99 && cos_angle_between_normals >= cos_angle_threshold)
        {
            // Reconstruct view-space position of the underlying geometry
            vec2 aspect = vec2(resolution.x / resolution.y, 1.0);
            vec2 xy_ndc = vec2(-1.0) + 2.0 * texel;
            float z_view = -(z_far - z_near) * zvn - z_near;
            vec2 xy_view = -z_view * tan_fovy_h * aspect * xy_ndc;

            // Transform into the DECAL BOX's object coordinates
            vec4 box = inverse(view * model) * vec4(xy_view, z_view, 1.0);

            // Remember: The box has coordinates from [-0.5, +0.5] on each axis
            if (abs(box.x) <= 0.5 && abs(box.y) <= 0.5 && abs(box.z) <= 0.5)
            {
                texel = box.xz + vec2(0.5);
                f_color = texture(tex_decal, texel);
            }
            else
            {
                f_color = vec4(1.0, 0.0, 0.0, 0.25);
            }
        }
        else
        {
            // Point was in the background
        }
    }
);

Shader shader_geometry, shader_decal;
RenderTexture rt_geometry;
Mesh mesh_cube, mesh_ssd;
uint tex_decal;
int window_width, window_height;

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;
    if (!shader_geometry.load_from_src(GEOMETRY_VS, GEOMETRY_FS) ||
        !shader_decal.load_from_src(DECAL_VS, DECAL_FS) ||
        !load_texture_from_file(tex_decal, "../data/textures/decal.png"))
        return false;
    rt_geometry = gen_rendertexture(window_width, window_height, GL_RGBA32F, GL_NEAREST, GL_NEAREST);
    return true;
}

void init_game()
{
    mesh_cube = Primitive::normal_cube();
    mesh_ssd = Primitive::cube();
    gfx2d::init(window_width, window_height);
}

void free_game()
{

}

void update_game(float dt)
{

}

vec3 raytrace_floor(float z_near, float z_far, float fov_y, mat4 view, vec2 pixel)
{
    vec2 texel = pixel / vec2(window_width, window_height);
    float x_ndc = -1.0f + 2.0f * texel.x;
    float y_ndc = 1.0f - 2.0f * texel.y;
    float aspect = window_width / float(window_height);
    float x_view = x_ndc * aspect * z_near * tan(fov_y / 2.0f);
    float y_view = y_ndc * z_near * tan(fov_y / 2.0f);
    float z_view = -z_near;

    mat4 inv_view = glm::inverse(view);
    vec3 ro = vec3(inv_view * vec4(0.0f, 0.0f, 0.0f, 1.0f));
    vec3 rd = normalize(vec3(inv_view * vec4(x_view, y_view, z_view, 0.0f)));
    float t = -ro.y / rd.y;
    return ro + rd * t;
}

vec2 project(vec3 p, mat4 projection, mat4 view)
{
    vec4 clip = projection * view * vec4(p.x, p.y, p.z, 1.0f);
    vec3 ndc = vec3(clip) / clip.w;
    vec2 pxc = vec2(0.5f) + 0.5f * vec2(ndc.x, ndc.y);
    pxc.y = 1.0f - pxc.y;
    pxc *= vec2(window_width, window_height);
    return pxc;
}

void debug_draw_3d_line(vec3 p0, vec3 p1, uint color, mat4 projection, mat4 view)
{
    vec2 pxc0 = project(p0, projection, view);
    vec2 pxc1 = project(p1, projection, view);
    gfx2d::draw_line(pxc0, pxc1, color);
}

void render_game(float dt)
{
    float z_near = 0.1f;
    float z_far = 8.0f;
    float fov_y = PI / 4.0f;
    mat4 mat_projection = perspective(fov_y, window_width / float(window_height), z_near, z_far);
    mat4 mat_view = translate(0.0f, 0.0f, -2.5f) * rotateX(-0.8f) * rotateY(0.0f);

    vec3 sprite_pos = raytrace_floor(z_near, z_far, fov_y, mat_view, get_mouse_pos());

    glBindFramebuffer(GL_FRAMEBUFFER, rt_geometry.fbo);
    blend_mode(false);
    cull(true);
    depth_test(true);
    depth_write(true);
    clear(0x000000ff, 1.0f);

    use_shader(shader_geometry);
    uniform("projection", mat_projection);
    uniform("view", mat_view);
    uniform("z_near", z_near);
    uniform("z_far", z_far);

    // Floor
    uniform("model", scale(5.0f, 0.1f, 5.0f));
    mesh_cube.draw();

    // Cube
    uniform("model", translate(0.0f, 0.0f, 0.0f) * rotateY(0.4f) * rotateX(-0.8f) * scale(0.55f));
    mesh_cube.draw();

    // Cube
    uniform("model", translate(-1.0f, 0.0f, -1.0f) * rotateY(-0.4f) * scale(2.0f, 4.0f, 0.2f));
    mesh_cube.draw();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, rt_geometry.fbo);
    glDrawBuffer(GL_BACK);
    clearc(0x000000ff);
    glBlitFramebuffer(0, 0, window_width, window_height,
                      0, 0, window_width, window_height,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    depth_write(false);
    depth_test(false);

    static float rotation_x = 0.0f;
    static float rotation_y = 0.0f;
    if (is_key_down('w'))       rotation_x += dt;
    else if (is_key_down('s'))  rotation_x -= dt;
    if (is_key_down('a'))       rotation_y += dt;
    else if (is_key_down('d'))  rotation_y -= dt;

    mat4 mat_model_decal = translate(sprite_pos) * rotateX(rotation_x) * rotateY(rotation_y);

    use_shader(shader_decal);
    blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    uniform("resolution", vec2(window_width, window_height));
    uniform("projection", mat_projection);
    uniform("view", mat_view);
    uniform("z_near", z_near);
    uniform("z_far", z_far);
    uniform("tan_fovy_h", tan(fov_y / 2.0f));
    uniform("model", mat_model_decal);
    uniform("tex_geometry", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, rt_geometry.color);
    uniform("tex_decal", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_decal);
    mesh_ssd.draw();

    // Draw lines around decal
    vec3 ll = vec3(-0.5f, 0.05f, -0.5f);
    vec3 ul = vec3(-0.5f, 0.05f, +0.5f);
    vec3 ur = vec3(+0.5f, 0.05f, +0.5f);
    vec3 lr = vec3(+0.5f, 0.05f, -0.5f);
    gfx2d::begin();
    glLineWidth(4.0f);
    debug_draw_3d_line(ll, ul, 0xff4411ff, mat_projection, mat_view * mat_model_decal);
    debug_draw_3d_line(ul, ur, 0xff4411ff, mat_projection, mat_view * mat_model_decal);
    debug_draw_3d_line(ur, lr, 0xff4411ff, mat_projection, mat_view * mat_model_decal);
    debug_draw_3d_line(lr, ll, 0xff4411ff, mat_projection, mat_view * mat_model_decal);
    debug_draw_3d_line(ll, ul, 0xff441144, mat_projection, mat_view * translate(sprite_pos));
    debug_draw_3d_line(ul, ur, 0xff441144, mat_projection, mat_view * translate(sprite_pos));
    debug_draw_3d_line(ur, lr, 0xff441144, mat_projection, mat_view * translate(sprite_pos));
    debug_draw_3d_line(lr, ll, 0xff441144, mat_projection, mat_view * translate(sprite_pos));
    gfx2d::end();
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }
