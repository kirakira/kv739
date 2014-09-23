#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h> 
#include <unistd.h>

#include "message.h"
#include "client.h"

using namespace std;

int fd = -1;

int kv739_init(char* server) {
    int colon = -1;
    for (size_t i = 0; colon == -1 && i < strlen(server); ++i)
        if (server[i] == ':')
            colon = i;
    if (colon == -1)
        return -1;
    int port = atoi(server + colon + 1);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return -1;
    int on;
    setsockopt (fd, SOL_TCP, TCP_NODELAY, &on, sizeof (on));

    char* server_address = new char[colon + 1]();
    memcpy(server_address, server, colon);

    struct hostent *host;
    host = gethostbyname(server_address);
    delete[] server_address;
    if (host == NULL)
        return -1;

    struct sockaddr_in serv_addr;
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) host->h_addr, 
         (char*) &serv_addr.sin_addr.s_addr,
         host->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        return -1;

    return 0;
}

int kv739_get(char* key, char* value) {
    if (fd < 0)
        return -1;

    Message request(key);
    request.Serialize(fd);
    Message response;
    if (!response.Deserialize(fd))
        return -1;
    if (response.ContainsValue()) {
        string response_value = response.value();
        memcpy(value, response_value.c_str(), 1 + response_value.length());
        return 0;
    } else
        return 1;
}

int kv739_put(char* key, char* value, char* old_value) {
    if (fd < 0)
        return -1;

    Message request(key, value);
    request.Serialize(fd);
    Message response;
    if (!response.Deserialize(fd))
        return 1;
    if (response.ContainsValue()) {
        string value = response.value();
        memcpy(old_value, value.c_str(), 1 + value.length());
        return 0;
    } else
        return 1;
}

int kv739_close() {
    if (fd < 0)
        return -1;
    close(fd);
    fd = -1;
    return 0;
}
