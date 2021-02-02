#pragma once
#include <vector>
#include <string>
#include <memory>

std::vector<std::string> splitString(const std::string& input, char delim = ';');

std::string& trim(std::string& s);
std::string trimCopy(const std::string& s);

std::string& removeAfter(std::string& s, char special);
std::string removeAfterCopy(const std::string& s, char special);

std::string toHexString(uint64_t value);

bool isRegister(const std::string& str, int numberOfRegisters);

std::shared_ptr<std::vector<std::string>> charPtrArrayToVector(char** inputArray, int length);
