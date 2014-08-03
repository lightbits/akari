#include "log.h"
#include <fstream>
#include <iostream>

Log APP_LOG("./log.txt", true);

Log::Log(std::string filename_, bool debug_) :
	buffer(""), filename(filename_), debug(debug_)
{ }

Log::~Log()
{
	std::ofstream file(filename);
	if (!file.is_open())
		return;

	file << buffer.str();
	file.close();

#ifdef _DEBUG
	std::cin.get();
#endif
}