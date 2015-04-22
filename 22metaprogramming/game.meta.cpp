#include "game.h"

#DeclareShader
    #name diffuse
    #vertex ./diffuse.vs
    #fragment ./diffuse.fs
    #attrib position vec3
    #attrib normal vec3
    #uniform albedo vec3
    #uniform projection mat4
    #uniform view mat4
    #uniform model mat4
#EndDeclareShader

int window_width;
int window_height;
float aspect_ratio;
Mesh mesh_cube;

bool load_game(int width, int height)
{
    window_width = width;
    window_height = height;
    aspect_ratio = width / (float)height;

    mesh_cube = Primitive::normal_cube();

    bool load_status = true;
    #LoadShader diffuse
    if (!load_status)
        return false;
	return true;
}

void free_game()
{

}

void init_game()
{

}

void update_game(float dt)
{

}

void render_game(float dt)
{
    mat4 mat_perspective = perspective(PI / 4.0f, aspect_ratio, 0.1f, 10.0f);
    mat4 mat_view = translate(0.0f, 0.0f, -2.0f) * rotateX(-0.5f) * rotateY(-0.3f);
    mat4 mat_model = rotateY(get_elapsed_time());

    depth_write(true);
    depth_test(true);
    clear(0x2a2a2aff, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, mesh_cube.vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_cube.index_buffer);

    #UseShader  diffuse
    #attrib     position
    #attrib     normal
    #albedo     = vec3(1.0, 0.7, 0.2)
    #projection = mat_projection
    #view       = mat_view
    #model      = mat_model

    glDrawElements(GL_TRIANGLES, mesh_cube.num_indices, GL_UNSIGNED_INT, 0);
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }
