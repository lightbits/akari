#include "shader.h"
#include "fileio.h"
#include "log.h"
#include <stdexcept>
#include <iostream>

bool compile_shader(uint shader, GLenum type, string src)
{
	const char *src_ptr = src.c_str();
    glShaderSource(shader, 1, &src_ptr, NULL);
    glCompileShader(shader);
    int status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        char *info = new char[length];
        glGetShaderInfoLog(shader, length, NULL, info);
		APP_LOG << "Error compiling shader: " << info << '\n';
        delete[] info;
		return false;
    }
    return true;
}

bool link_program(uint program, vector<uint> shaders)
{
	for (int i = 0; i < shaders.size(); i++)
		glAttachShader(program, shaders[i]);

    glLinkProgram(program);

	for (int i = 0; i < shaders.size(); i++)
	{
		glDetachShader(program, shaders[i]);
		glDeleteShader(shaders[i]);
	}

    int status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *info = new char[length];
        glGetProgramInfoLog(program, length, NULL, info);
		APP_LOG << "Error linking program: " << info << '\n';
        delete[] info;
		return false;
    }
    return true;
}

bool Shader::load_from_src(string *sources, GLenum *types, int count)
{
	m_id = glCreateProgram();
	vector<uint> shaders;
	for (int i = 0; i < count; i++)
	{
		uint shader = glCreateShader(types[i]);
		if (!compile_shader(shader, types[i], sources[i].c_str()))
			return false;
		shaders.push_back(shader);
	}
	return link_program(m_id, shaders);
}

bool Shader::load_from_src(string vs_src, string fs_src)
{
	string sources[] = { vs_src, fs_src };
	GLenum types[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	return load_from_src(sources, types, 2);
}

bool Shader::load_from_file(string *paths, GLenum *types, int count)
{
	vector<string> sources;
	for (int i = 0; i < count; i++)
	{
		string src;
		if (!read_file(paths[i], src))
			return false;
		sources.push_back(src);
	}
	return load_from_src(&sources[0], types, count);
}

bool Shader::load_from_file(string vs_path, string fs_path)
{
	string paths[] = { vs_path, fs_path };
	GLenum types[] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
	return load_from_file(paths, types, 2);
}

void Shader::dispose()
{
	m_attributes.clear();
	m_uniforms.clear();
	glDeleteProgram(m_id);
}

void Shader::use()
{
	glUseProgram(m_id);
}

void Shader::unuse()
{
	glUseProgram(0);
}

GLint Shader::get_uniform_location(string name)
{
	std::unordered_map<std::string, GLint>::iterator it = m_uniforms.find(name);
	if(it != m_uniforms.end())
	{
		return it->second;
	}
	else
	{
		GLint location = glGetUniformLocation(m_id, name.c_str());
		WARN(location >= 0, "Invalid shader uniform [" + name + "] (not used or bad name)");
		m_uniforms[name] = location;
		return location;
	}
}

GLint Shader::get_attribute_location(string name)
{
	std::unordered_map<std::string, GLint>::iterator it = m_attributes.find(name);
	if(it != m_attributes.end())
	{
		return it->second;
	}
	else
	{
		GLint location = glGetAttribLocation(m_id, name.c_str());
		WARN(location >= 0, "Invalid shader attribute [" + name + "] (not used or bad name)");
		m_attributes[name] = location;
		return location;
	}
}

void Shader::set_attribfv(string name, GLsizei num_components, 
						  GLsizei stride, GLsizei offset)
{
	GLint loc = get_attribute_location(name);
	glEnableVertexAttribArray(loc);
	glVertexAttribPointer(
		loc, 
		num_components, // in the attribute,
		GL_FLOAT, // each component is a float,
		GL_FALSE, // not normalized,
		stride * sizeof(GLfloat), // the attribs are seperated by this much,
		(void*)(offset * sizeof(GLfloat)) // beginning at this offset in the array.
		);
}

void Shader::unset_attrib(string name)
{
	glDisableVertexAttribArray(get_attribute_location(name));
}

void Shader::set_uniform(string name, const mat4 &v) { glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(v)); }
void Shader::set_uniform(string name, const mat3 &v) { glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(v)); }
void Shader::set_uniform(string name, const mat2 &v) { glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(v)); }
void Shader::set_uniform(string name, const vec4 &v) { glUniform4f(get_uniform_location(name), v.x, v.y, v.z, v.w); }
void Shader::set_uniform(string name, const vec3 &v) { glUniform3f(get_uniform_location(name), v.x, v.y, v.z); }
void Shader::set_uniform(string name, const vec2 &v) { glUniform2f(get_uniform_location(name), v.x, v.y); }
void Shader::set_uniform(string name, double v) { glUniform1f(get_uniform_location(name), v); }
void Shader::set_uniform(string name, float v) { glUniform1f(get_uniform_location(name), v); }
void Shader::set_uniform(string name, int v) { glUniform1i(get_uniform_location(name), v); }
void Shader::set_uniform(string name, uint v) { glUniform1ui(get_uniform_location(name), v); }