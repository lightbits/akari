#include "game.h"

struct ProgramDiffuse 
{
    GLuint handle;
    GLint position;
    GLint normal;
    GLint model;
    GLint view;
    GLint albedo;
    GLint projection;

};
ProgramDiffuse program_diffuse;

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

string program_diffuse_paths[] = { "./diffuse.vs", "./diffuse.fs" };
GLenum program_diffuse_types[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
status &= load_shader_from_file(program_diffuse_paths, program_diffuse_types, 2);

program_diffuse.position = glGetAttribLocation(program_diffuse, "position");
program_diffuse.normal = glGetAttribLocation(program_diffuse, "normal");

program_diffuse.model = glGetUniformLocation(program_diffuse, "model");
program_diffuse.view = glGetUniformLocation(program_diffuse, "view");
program_diffuse.albedo = glGetUniformLocation(program_diffuse, "albedo");
program_diffuse.projection = glGetUniformLocation(program_diffuse, "projection");

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


    glDrawElements(GL_TRIANGLES, mesh_cube.num_indices, GL_UNSIGNED_INT, 0);
}

void on_key_up(uint16 mod, SDL_Keycode key) { }
void on_key_down(uint16 mod, SDL_Keycode key) { }
void on_mouse_moved(int x, int y, int dx, int dy) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy) { }
void on_mouse_pressed(int button, int x, int y) { }
void on_mouse_released(int button, int x, int y) { }
