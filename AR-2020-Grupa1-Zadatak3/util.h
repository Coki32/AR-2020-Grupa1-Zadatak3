#pragma once
#include <vector>
#include <string>

std::vector<std::string> splitString(const std::string& input, char delim = ';');

std::string& trim(std::string& s);
std::string trimCopy(const std::string& s);

std::string& removeAfter(std::string& s, char special);
std::string removeAfterCopy(const std::string& s, char special);

bool isRegister(const std::string& str, int numberOfRegisters);