#include "util.h"
#include <sstream>

std::vector<std::string> splitString(const std::string& input, char delim)
{
	std::vector<std::string> strings;
	std::string part;
	std::stringstream ss(input);
	while (std::getline(ss, part, delim))
		strings.push_back(part);
	return strings;
}


static bool notASpace(char c)
{
	return !std::isspace(c);
}

static void trimLeft(std::string& s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), notASpace));
}

static void trimRight(std::string& s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), notASpace).base(), s.end());
}

std::string& trim(std::string& s)
{
	trimLeft(s);
	trimRight(s);
	return s;
}

std::string trimCopy(const std::string& s)
{
	std::string copy(s);
	return trim(copy);
}

std::string& removeAfter(std::string& s, char special)
{
	s.erase(std::find(s.begin(), s.end(), special), s.end());
	return s;
}

std::string removeAfterCopy(const std::string& s, char special)
{
	std::string copy = std::string(s);
	return removeAfter(copy, special);
}

bool isRegister(const std::string& str, int numberOfRegisters)
{
	int len = str.length();
	if (len < 2 || len>3)
		return false;
	if (len == 2) {
		if ((str[1] == 'x' || str[1] == 'l') && (str[0] - 'a') < numberOfRegisters)
			return true;
		else
			return false;
	}//else znaci len==3
	if ((str[0] != 'r' && str[0] != 'e') || str[2]!='x')//mora biti [r|e][a..(a+numberOfRegisters)]x
		return false;
	if ((str[1] - 'a') >= numberOfRegisters)
		return false;
	return true;
}

