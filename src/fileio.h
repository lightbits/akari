#ifndef FILE_IO_H
#define FILE_IO_H
#include <string>
#include <sstream>

bool read_file(const std::string &path, std::string &dest);
int read_int(std::stringstream &ss);
bool read_bool(std::string &ss);
std::string read_word(std::stringstream &ss);

#endif