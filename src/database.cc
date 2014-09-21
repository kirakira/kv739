#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "database.h"
#include "message.h"

using namespace std;

const mode_t mode644 = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_ISUID | S_ISGID;

void database_init(const string& file, map<string, string>* table) {
    table->clear();

    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1)
        return;

    while (true) {
        Message request;
        if (!request.Deserialize(fd))
            break;
        (*table)[request.key()] = request.value();
    }

    close(fd);

    fd = open(file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, mode644);
    for (map<string, string>::iterator iter = table->begin();
            iter != table->end();
            ++iter) {
        Message request(iter->first, iter->second);
        request.Serialize(fd);
    }
    close(fd);
}

void database_put(const string& file, const string& key, const string& value) {
    int fd = open(file.c_str(), O_WRONLY | O_APPEND | O_CREAT, mode644);
    Message request(key, value);
    request.Serialize(fd);
    close(fd);
}
