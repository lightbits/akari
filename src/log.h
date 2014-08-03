#ifndef APP_LOG_H
#define APP_LOG_H
#include <sstream>
#include <string>
#include <iostream>

/*
A stream meant for logging errors, warnings and status messages.
It writes its contents to the filename given upon destruction,
and waits for input upon destruction if the 'debug' parameter is enabled.
It also prints to cout.
*/
class Log
{
public:
	Log(std::string filename, bool debug);
	~Log();

	template <typename T>
	Log &operator<<(T t)
	{
		buffer << t;
		std::cout << t;
		return *this;
	}
private:
	bool debug;
	std::string filename;
	std::stringstream buffer;
};

// Global log file
// Defined in log.cpp
extern Log APP_LOG;

#ifndef NDEBUG
#define ASSERT(condition, message) \
	do { \
		if (!(condition)) { \
			APP_LOG << "[Error] " << message << '\n'; \
			std::exit(EXIT_FAILURE); \
		}\
	} while (false)

#define WARN(condition, message) \
	do { \
		if (!(condition)) { \
			APP_LOG << "[Warning] " << message << '\n'; \
		} \
	} while (false) 
#else
#define ASSERT(condition, message) do { } while (false)
#define WARN(condition, message) do { } while (false)
#endif

#endif