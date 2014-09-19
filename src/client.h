#ifndef CLIENT_H_
#define CLIENT_H_

extern int kv739_init(char* server);
extern int kv739_get(char* key, char* value);
extern int kv739_put(char* key, char* value, char* old_value);
extern int kv739_close();

#endif  // CLIENT_H_
