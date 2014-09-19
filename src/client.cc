#include <string>

#include "message.h"
#include "client.h"

using namespace std;

int fd = -1;

int kv739_get(char* key, char* value) {
    if (fd == -1)
        return -1;

    Message request(key);
    request.Serialize(fd);
    Message response(fd);
    if (response.ContainsValue()) {
        string response_value = response.value();
        memcpy(value, response_value.c_str(), 1 + response_value.length());
        return 0;
    } else
        return 1;
}

int kv739_put(char* key, char* value, char* old_value) {
    if (fd == -1)
        return -1;

    Message request(key, value);
    request.Serialize(fd);
    Message response(fd);
    if (response.ContainsValue()) {
        string value = response.value();
        memcpy(old_value, value.c_str(), 1 + value.length());
        return 0;
    } else
        return 1;
}
