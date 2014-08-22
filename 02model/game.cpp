#include "game.h"

Shader 
	shader_font,
	shader_cube,
	shader_sponza;
Font 
	font_debug;
Mesh 
	mesh_sponza, 
	mesh_cube,
	mesh_body;
mat4 
	mat_ortho,
	mat_perspective,
	mat_view;
Camera
	camera;
int window_width,
	window_height;

//bool read_int(const char **s, int &i)
//{
//	char c = **s;
//	while ((**s) != '\0' && !isdigit(**s))
//		(*s)++;
//	if (**s == '\0')
//		return false;
//	i = 0;
//	while ((**s) != '\0' && isdigit(**s))
//	{
//		i *= 10;
//		i += int(**s) - int('0');
//		(*s)++;
//	}
//	return true;
//}
//
//void process_point(const string &point, vector<vec3> &vp, vector<vec2> &vt, vector<vec3> &vn, vector<float> &vertices)
//{
//	int index;
//	const char *c = &point[0];
//	read_int(&c, index);
//	vertices.push_back(vp[index - 1].x);
//	vertices.push_back(vp[index - 1].y);
//	vertices.push_back(vp[index - 1].z);
//	if (vt.size() > 0)
//	{
//		read_int(&c, index);
//		vertices.push_back(vt[index - 1].x);
//		vertices.push_back(vt[index - 1].y);
//	}
//	if (vn.size() > 0)
//	{
//		read_int(&c, index);
//		vertices.push_back(vn[index - 1].x);
//		vertices.push_back(vn[index - 1].y);
//		vertices.push_back(vn[index - 1].z);
//	}
//}
//
//void process_vertex(const string &vertex, 
//				   unordered_map<string, uint32> &unique_vertices, 
//				   vector<vec3> &vp, 
//				   vector<vec2> &vt, 
//				   vector<vec3> &vn, 
//				   vector<float> &vertices,
//				   vector<uint32> &indices,
//				   uint32 &num_vertices)
//{
//	// The vertex already exists, we only need to add its index
//	if (unique_vertices.find(vertex) != unique_vertices.end())
//	{
//		indices.push_back(unique_vertices.find(vertex)->second);
//		return;
//	}
//
//	// Otherwise we parse the vertex string and append the vertex data
//	unique_vertices[vertex] = num_vertices;
//	indices.push_back(num_vertices);
//	int index;
//	const char *c = &vertex[0];
//	read_int(&c, index);
//	vertices.push_back(vp[index - 1].x);
//	vertices.push_back(vp[index - 1].y);
//	vertices.push_back(vp[index - 1].z);
//	if (vt.size() > 0)
//	{
//		read_int(&c, index);
//		vertices.push_back(vt[index - 1].x);
//		vertices.push_back(vt[index - 1].y);
//	}
//	if (vn.size() > 0)
//	{
//		read_int(&c, index);
//		vertices.push_back(vn[index - 1].x);
//		vertices.push_back(vn[index - 1].y);
//		vertices.push_back(vn[index - 1].z);
//	}
//	num_vertices++;
//}
//
//bool load_mesh(Mesh &mesh, string path)
//{
//	string file;
//	if (!read_file(path, file))
//		return false;
//
//	vector<float> vertices;
//
//	stringstream file_stream(file);
//	string line;
//	vector<vec3> positions;
//	vector<vec2> texels;
//	vector<vec3> normals;
//	while (std::getline(file_stream, line))
//	{
//		stringstream ss(line);
//		string word; ss >> word;
//		if (word == "v")
//		{
//			vec3 v; ss >> v.x; ss >> v.y; ss >> v.z;
//			positions.push_back(v);
//		}
//		else if (word == "vt")
//		{
//			vec2 vt; ss >> vt.x; ss >> vt.y;
//			texels.push_back(vt);
//		}
//		else if (word == "vn")
//		{
//			vec3 vn; ss >> vn.x; ss >> vn.y; ss >> vn.z;
//			normals.push_back(vn);
//		}
//		else if (word == "f")
//		{
//			vector<string> points = split<string>(line, ' ');
//			if (points.size() == 5)
//			{
//				process_point(points[1], positions, texels, normals, vertices);
//				process_point(points[2], positions, texels, normals, vertices);
//				process_point(points[3], positions, texels, normals, vertices);
//				process_point(points[3], positions, texels, normals, vertices);
//				process_point(points[4], positions, texels, normals, vertices);
//				process_point(points[1], positions, texels, normals, vertices);
//			}
//			else
//			{
//				process_point(points[1], positions, texels, normals, vertices);
//				process_point(points[2], positions, texels, normals, vertices);
//				process_point(points[3], positions, texels, normals, vertices);
//			}
//		}
//		else
//		{
//			// Not handled yet
//		}
//	}
//
//	APP_LOG << "Loaded model " << get_filename(path) << "\n";
//	APP_LOG << positions.size() << " vertices\n";
//	APP_LOG << texels.size() << " texels\n";
//	APP_LOG << normals.size() << " normals\n";
//	APP_LOG << vertices.size() * sizeof(float) << " bytes in total\n";
//
//	bool has_texels = texels.size() > 0;
//	bool has_normals = normals.size() > 0;
//
//	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0]);
//	mesh.index_buffer = 0;
//	mesh.num_indices = 0;
//	mesh.num_vertices = vertices.size();
//	mesh.draw = [=]() {
//		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
//		attribfv_append("position", 3);
//		if (has_texels) attribfv_append("texel", 2);
//		if (has_normals) attribfv_append("normal", 3);
//		attribfv_enable();
//		glDrawArrays(GL_TRIANGLES, 0, mesh.num_vertices);
//	};
//
//	return true;
//}
//
//bool load_mesh_indexed(Mesh &mesh, string path)
//{
//	string file;
//	if (!read_file(path, file))
//		return false;
//
//	vector<float> vertices;
//	vector<uint32> indices;
//	unordered_map<string, uint32> unique_vertices;
//
//	stringstream file_stream(file);
//	string line;
//	vector<vec3> positions;
//	vector<vec2> texels;
//	vector<vec3> normals;
//	uint32 num_vertices = 0;
//	uint32 progress = 0;
//	uint32 filesize = file.size();
//	APP_LOG << "Processing |";
//	while (std::getline(file_stream, line))
//	{
//		stringstream ss(line);
//		string word; ss >> word;
//		if (word == "v")
//		{
//			vec3 v; ss >> v.x; ss >> v.y; ss >> v.z;
//			positions.push_back(v);
//		}
//		else if (word == "vt")
//		{
//			vec2 vt; ss >> vt.x; ss >> vt.y;
//			texels.push_back(vt);
//		}
//		else if (word == "vn")
//		{
//			vec3 vn; ss >> vn.x; ss >> vn.y; ss >> vn.z;
//			normals.push_back(vn);
//		}
//		else if (word == "f")
//		{
//			vector<string> points = split<string>(line, ' ');
//			if (points.size() == 5)
//			{
//				process_vertex(points[1], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[2], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[3], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[3], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[4], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[1], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//			}
//			else
//			{
//				process_vertex(points[1], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[2], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//				process_vertex(points[3], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
//			}
//		}
//		else
//		{
//			// Not handled yet
//		}
//
//		progress += line.size();
//		if (((100 * progress) / (filesize - 1)) >= 5)
//		{
//			progress = 0;
//			APP_LOG << "-";
//		}
//	}
//	APP_LOG << "|\n";
//
//	APP_LOG << "Loaded model " << get_filename(path) << "\n";
//	APP_LOG << positions.size() << " vertices\n";
//	APP_LOG << texels.size() << " texels\n";
//	APP_LOG << normals.size() << " normals\n";
//	APP_LOG << vertices.size() * sizeof(float) << " bytes in total\n";
//
//	bool has_texels = texels.size() > 0;
//	bool has_normals = normals.size() > 0;
//
//	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0]);
//	mesh.index_buffer = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), &indices[0]);
//	mesh.num_indices = indices.size();
//	mesh.num_vertices = vertices.size();
//	mesh.draw = [=]() {
//		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
//		attribfv_append("position", 3);
//		if (has_texels) attribfv_append("texel", 2);
//		if (has_normals) attribfv_append("normal", 3);
//		attribfv_enable();
//		glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
//	};
//
//	return true;
//}
//
//bool load_mesh_binary(Mesh &mesh, string path)
//{
//	std::ifstream in(path, std::ios::in | std::ios::binary);
//	if(!in.is_open())
//	{
//		APP_LOG << "Could not open file: " << path << '\n';
//		return false;
//	}
//
//	if(!in.good())
//		return false;
//
//	in.seekg(0, std::ios::end);
//	uint32 size = in.tellg();
//	char *data = new char[size];
//	in.seekg(0, std::ios::beg);
//	in.read(data, size);
//	in.close();
//
//	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
//	mesh.index_buffer = 0;
//	mesh.num_indices = 0;
//}
//
//void dump_mesh_binary(Mesh &mesh, string path)
//{
//
//	int size;
//	glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
//	glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
//
//	uint8 *data = (uint8*)glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_READ_BIT);
//	std::ofstream out(path);
//	for (uint32 i = 0; i < size; i++)
//		out << data[i];
//	out.close();
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//}

bool load_game(int width, int height)
{
	window_width = width;
	window_height = height;

	APP_LOG << split<string>("f 1/1 2/2 3/3 \r", ' ').size() << '\n';

	if (!shader_font.load_from_file("../data/shaders/font.vs", "../data/shaders/font.fs") ||
		!shader_cube.load_from_file("./cube.vs", "./cube.fs") ||
		!shader_sponza.load_from_file("./sponza.vs", "./sponza.fs"))
		return false;

	if (!load_font(font_debug, "../data/fonts/proggytinyttsz_8x12.png"))
		return false;

	//if (!load_mesh_indexed(mesh_sponza, "../../../resources/models/crytek-sponza/sponza.obj"))
	//	return false;
	if (!load_mesh_indexed(mesh_sponza, "../data/models/cube.obj"))
		return false;

	if (!load_mesh_indexed(mesh_cube, "../data/models/cube.obj"))
		return false;

	if (!load_mesh_indexed(mesh_body, "../data/models/body.obj"))
		return false;

	return true;
}

void free_game()
{
	shader_font.dispose();
	shader_cube.dispose();
	mesh_sponza.dispose();
	mesh_cube.dispose();
	font_debug.dispose();
}

void init_game()
{
	mat_ortho = ortho(0.0f, float(window_width), float(window_height), 0.0f, 0.0f, 1.0f);
	mat_perspective = perspective(PI / 4.0f, window_width / float(window_height), 0.1f, 5.0f);

	camera.setHorizontalAngle(-PI);
	camera.setVerticalAngle(0.0f);
	camera.setPosition(vec3(0.0f, 0.0f, -2.0f));
}

vec3 camera_pos = vec3(0.0f);
void update_game(float dt)
{
	if (is_key_down('a')) camera.moveLeft(dt);
	else if (is_key_down('d')) camera.moveRight(dt);
	if (is_key_down('w')) camera.moveForward(dt);
	else if (is_key_down('s')) camera.moveBackward(dt);
	if (is_key_down(SDLK_LCTRL)) camera.moveDown(dt);
	else if (is_key_down(SDLK_SPACE)) camera.moveUp(dt);

	if (is_mouse_down(MouseButtonLeft))
	{
		vec2i m = get_mouse_pos();
		float dx = (m.x - window_width / 2) / (float)window_width;
		float dy = (m.y - window_height / 2) / (float)window_height;
		float speed = 5.0f;
		camera.rotateHorizontal(speed * sqrt(abs(dx)) * dx * dt);
		camera.rotateVertical(speed * sqrt(abs(dy)) * dy * dt);
	}

	float t = get_elapsed_time() * 0.25f;
	mat_view = camera.getViewMatrix();
}

void render_game(float dt)
{
	blend_mode(false);
	depth_test(true, GL_LEQUAL);
	depth_write(true);
	clear(0x2a2a2aff, 1.0);

	use_shader(shader_cube);
	uniform("projection", mat_perspective);
	uniform("view", mat_view);
	//uniform("model", scale(1.0f / 1000.0f));
	uniform("model", scale(0.01f));
	uniform("time", get_elapsed_time());
	//mesh_sponza.draw();
	mesh_body.draw();

	use_shader(shader_cube);
	uniform("projection", mat_perspective);
	uniform("view", mat_view);
	uniform("time", get_elapsed_time());

	uniform("model", translate(-0.1f, 1.1f, 0.0f) * rotateX(-0.2f) * scale(0.20f));
	mesh_cube.draw();

	uniform("model", translate(0.0f, -0.5f, 0.0f) * scale(1.0f, 0.05f, 1.0f));
	mesh_cube.draw();

	//use_shader(shader_font);
	//use_font(font_debug);
	//uniform("projection", mat_ortho);
	//blend_mode(true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//draw_string(5.0f, 5.0f, "frametime: " + std::to_string((long double)dt * 1000.0f) + "ms");
}

void on_key_up(uint16 mod, SDL_Keycode key, float dt) { }
void on_key_down(uint16 mod, SDL_Keycode key, float dt) { }
void on_mouse_moved(int x, int y, int dx, int dy, float dt) { }
void on_mouse_dragged(int button, int x, int y, int dx, int dy, float dt) { }
void on_mouse_pressed(int button, int x, int y, float dt) { }
void on_mouse_released(int button, int x, int y, float dt) { }