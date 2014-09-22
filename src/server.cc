#include <iostream>
#include <map>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <unistd.h>

#include "database.h"
#include "message.h"

using namespace std;

int port = 8000;
string database_file = "kv.db";

void error(const char* str) {
    cerr << str << endl;
}

map<string, string> database;

bool put(const string& key, const string& value, string* old_value) {
    bool ret = false;
    if (database.count(key) != 0) {
        *old_value = database[key];
        ret = true;
    }
    database[key] = value;
    database_put(database_file, key, value);
    return ret;
}

bool get(const string& key, string* value) {
    if (database.count(key) > 0) {
        *value = database[key];
        return true;
    } else
        return false;
}

void handle_client(int fd) {
    Message request;
    while (true) {
        if (!request.Deserialize(fd))
            break;
        if (request.ContainsValue()) {
            string old_value;
            if (put(request.key(), request.value(), &old_value)) {
                Message response(request.key(), old_value);
                if (!response.Serialize(fd))
                    break;
            } else {
                Message response(request.key());
                if (!response.Serialize(fd))
                    break;
            }
        } else {
            string value;
            if (get(request.key(), &value)) {
                Message response(request.key(), value);
                if (!response.Serialize(fd))
                    break;
            } else {
                Message response(request.key());
                if (!response.Serialize(fd))
                    break;
            }
        }
    }
}

int main() {
    database_init(database_file, &database);
    cout << "Initialized a database with " << database.size() << " entries." << endl;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error("ERROR opening socket");
        return -1;
    }

    struct sockaddr_in serv_addr, cli_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *) &serv_addr,
                sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
        return -1;
    }

    listen(fd,5);
    cout << "Listening..." << endl;
    socklen_t clilen = sizeof(cli_addr);
    while (true) {
        int newsockfd = accept(fd, 
                (struct sockaddr *) &cli_addr, 
                &clilen);
        cout << "New client" << endl;
        if (newsockfd < 0) {
            error("ERROR on accept");
            return -1;
        }

        handle_client(newsockfd);

        close(newsockfd);
    }
    close(fd);
    return 0;
}
