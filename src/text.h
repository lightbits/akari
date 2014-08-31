#ifndef TEXT_H
#define TEXT_H
#include <sstream>
#include <string>

class Text
{
public:
	Text()
	{
		stream = std::stringstream("");
	}

	Text &operator=(const std::string &str_)
	{
		stream = std::stringstream(str_);
		return *this;
	}

	std::string getString() const
	{
		return stream.str();
	}

	template <typename T>
	Text &operator<<(const T &rhs)
	{
		stream<<rhs;
		return *this;
	}
private:
	std::stringstream stream;
};

#endif