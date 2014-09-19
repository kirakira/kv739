#ifndef COMMON_H
#define COMMON_H

#include <string>

int WriteN(int fd, const char* buffer, int n);
int ReadN(int fd, char* buffer, int n);
bool WriteString(int fd, const std::string& str);
bool ReadString(int fd, std::string* str);

#endif  // COMMON_H
