#include "fileio.h"
#include "log.h"
#include <fstream>
#include <iostream>

bool read_file(const std::string &path, std::string &dest)
{
	std::ifstream in(path, std::ios::in | std::ios::binary);
	if(!in.is_open())
	{
		APP_LOG << "Could not open file: " << path << '\n';
		return false;
	}

	APP_LOG << "Opened file " << path << '\n';

	if(in.good())
	{
		in.seekg(0, std::ios::end);			// Set get position to end
		dest.resize(in.tellg());			// Resize string to support enough bytes
		in.seekg(0, std::ios::beg);			// Set get position to beginning
		in.read(&dest[0], dest.size());		// Read file to string
		in.close();
	}

	return true;
}

int read_int(std::stringstream &ss)
{
	int i; ss>>i;
	return i;
}

bool read_bool(std::stringstream &ss)
{
	bool i; ss>>i;
	return i;
}

std::string read_word(std::stringstream &ss)
{
	std::string word; ss>>word;
	return word;
}

std::string get_filename(const std::string &path)
{
	std::size_t i = path.find_last_of('/');
	return path.substr(i + 1, path.length() - i);
}