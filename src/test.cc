#include <iostream>

#include "client.h"

using namespace std;

int main() {
    cout << kv739_init("127.0.0.1:8000") << endl;
    char buffer[50000];
    cout << kv739_put("aa", "bb", buffer) << endl;
    cout << kv739_get("aa", buffer) << endl;
    cout << buffer << endl;
    return 0;
}
