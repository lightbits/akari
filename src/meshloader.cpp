#include "meshloader.h"
#include "fileio.h"
#include "log.h"
#include "gfx.h"

bool read_int(const char **s, int &i)
{
	char c = **s;
	while ((**s) != '\0' && !isdigit(**s))
		(*s)++;
	if (**s == '\0')
		return false;
	i = 0;
	while ((**s) != '\0' && isdigit(**s))
	{
		i *= 10;
		i += int(**s) - int('0');
		(*s)++;
	}
	return true;
}

void process_vertex(const string &vertex, 
				   unordered_map<string, uint32> &unique_vertices, 
				   vector<vec3> &vp, 
				   vector<vec2> &vt, 
				   vector<vec3> &vn, 
				   vector<float> &vertices,
				   vector<uint32> &indices,
				   uint32 &num_vertices)
{
	// The vertex already exists, we only need to add its index
	if (unique_vertices.find(vertex) != unique_vertices.end())
	{
		indices.push_back(unique_vertices.find(vertex)->second);
		return;
	}

	// Otherwise we parse the vertex string and append the vertex data
	unique_vertices[vertex] = num_vertices;
	indices.push_back(num_vertices);
	int index;
	const char *c = &vertex[0];
	read_int(&c, index);
	vertices.push_back(vp[index - 1].x);
	vertices.push_back(vp[index - 1].y);
	vertices.push_back(vp[index - 1].z);
	if (vt.size() > 0)
	{
		read_int(&c, index);
		vertices.push_back(vt[index - 1].x);
		vertices.push_back(vt[index - 1].y);
	}
	if (vn.size() > 0)
	{
		read_int(&c, index);
		vertices.push_back(vn[index - 1].x);
		vertices.push_back(vn[index - 1].y);
		vertices.push_back(vn[index - 1].z);
	}
	num_vertices++;
}

bool load_mesh_indexed(Mesh &mesh, string path)
{
	string file;
	if (!read_file(path, file))
		return false;

	vector<float> vertices;
	vector<uint32> indices;
	unordered_map<string, uint32> unique_vertices;

	stringstream file_stream(file);
	string line;
	vector<vec3> positions;
	vector<vec2> texels;
	vector<vec3> normals;
	uint32 num_vertices = 0;
	uint32 progress = 0;
	uint32 filesize = file.size();
	APP_LOG << "Processing |";
	while (std::getline(file_stream, line))
	{
		stringstream ss(line);
		string word; ss >> word;
		if (word == "v")
		{
			vec3 v; ss >> v.x; ss >> v.y; ss >> v.z;
			positions.push_back(v);
		}
		else if (word == "vt")
		{
			vec2 vt; ss >> vt.x; ss >> vt.y;
			texels.push_back(vt);
		}
		else if (word == "vn")
		{
			vec3 vn; ss >> vn.x; ss >> vn.y; ss >> vn.z;
			normals.push_back(vn);
		}
		else if (word == "f")
		{
			vector<string> points = split<string>(line, ' ');
			if (points.size() == 5)
			{
				process_vertex(points[1], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[2], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[3], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[3], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[4], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[1], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
			}
			else
			{
				process_vertex(points[1], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[2], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
				process_vertex(points[3], unique_vertices, positions, texels, normals, vertices, indices, num_vertices);
			}
		}
		else
		{
			// Not handled yet
		}

		progress += line.size();
		if (((100 * progress) / (filesize - 1)) >= 5)
		{
			progress = 0;
			APP_LOG << "-";
		}
	}
	APP_LOG << "|\n";

	APP_LOG << "Loaded model " << get_filename(path) << "\n";
	APP_LOG << positions.size() << " vertices\n";
	APP_LOG << texels.size() << " texels\n";
	APP_LOG << normals.size() << " normals\n";
	APP_LOG << vertices.size() * sizeof(float) << " bytes in total\n";

	bool has_texels = texels.size() > 0;
	bool has_normals = normals.size() > 0;

	mesh.vertex_buffer = gen_buffer(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0]);
	mesh.index_buffer = gen_buffer(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32), &indices[0]);
	mesh.num_indices = indices.size();
	mesh.num_vertices = vertices.size();
	mesh.draw = [=]() {
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_buffer);
		attribfv_append("position", 3);
		if (has_texels) attribfv_append("texel", 2);
		if (has_normals) attribfv_append("normal", 3);
		attribfv_enable();
		glDrawElements(GL_TRIANGLES, mesh.num_indices, GL_UNSIGNED_INT, 0);
	};

	return true;
}