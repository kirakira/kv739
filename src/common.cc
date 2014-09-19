#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "common.h"

using namespace std;

int WriteN(int fd, const char* buffer, int n) {
    int sent = 0;
    while (sent < n) {
        int t = write(fd, buffer + sent, n - sent);
        if (t <= 0)
            return t;
        sent += t;
    }
    return n;
}

int ReadN(int fd, char* buffer, int n) { 
    int received = 0;
    while (received < n) {
        int t = read(fd, buffer + received, n - received);
        if (t <= 0)
            return 0;
        received += t;
    }
    return n;
}

bool WriteString(int fd, const string& str) {
    int len = str.length();
    if (WriteN(fd, (const char*) &len, sizeof(len)) <= 0)
        return false;
    if (len > 0 && WriteN(fd, str.c_str(), len) <= 0)
        return false;
    return true;
}

bool ReadString(int fd, string* str) {
    int len;
    if (ReadN(fd, (char*) &len, sizeof(len)) <= 0)
        return false;
    char* buffer = new char[len];
    if (len > 0 && ReadN(fd, buffer, len) <= 0)
        return false;
    *str = string(buffer, len);
    delete[] buffer;
    return true;
}
