#include <iostream>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <set>
#include <signal.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <unistd.h>

#include "client.h"
#include "database.h"
#include "message.h"

using namespace std;

string persistence_test_file = "kv_test_storage.db";

char* BadStringConversion(const char* s) {
    int len = strlen(s);
    char* buffer = new char[len + 1];
    strcpy(buffer, s);
    return buffer;
}

char* BadStringConversion(const string& s) {
    return BadStringConversion(s.c_str());
}

char buffer[50000];

bool TestNaive() {
    if (kv739_put(BadStringConversion("aa"), BadStringConversion("bb"), buffer) == -1) {
        cout << "Test naive failed at 1" << endl;
        return false;
    }
    if (kv739_get(BadStringConversion("aa"), buffer) != 0) {
        cout << "Test naive failed at 2" << endl;
        return false;
    }
    if (strcmp(buffer, "bb") != 0) {
        cout << "Test naive failed at 3" << endl;
        return false;
    }
    return true;
}

string RandomString(int length) {
    char candidates[] = {"abcdefghijklmnopqrstuvwxyz1234567890"};
    string ret;
    for (int i = 0; i < length; ++i)
        ret += candidates[rand() % (sizeof(candidates) - 1)];
    return ret;
}

bool TestPersistence() {
    int fd = open(persistence_test_file.c_str(), O_RDONLY);
    map<string, string> table;
    while (true) {
        Message request;
        if (!request.Deserialize(fd))
            break;
        table[request.key()] = request.value();
    }
    close(fd);

    for (map<string, string>::iterator iter = table.begin();
            iter != table.end();
            ++iter) {
        if (kv739_get(BadStringConversion(iter->first), buffer) != 0)
            return false;
        if (iter->second != buffer)
            return false;
    }
    return true;
}

bool TestCorrectness() {
    map<string, string> table;
    set<string> keys;
    int num_keys = 100;
    for (int i = 0; i < num_keys; ++i) {
        while (true) {
            string k = RandomString(4);
            if (keys.count(k) > 0)
                continue;
            keys.insert(k);
            if (kv739_get(BadStringConversion(k), buffer) == 0) {
                table[k] = buffer;
                cout << "premap " << k << " to " << buffer << endl;
            }
            break;
        }
    }

    vector<string> keys_list(keys.begin(), keys.end());
    int num_operations = 10000;
    for (int i = 0; i < num_operations; ++i) {
        string key = keys_list[rand() % keys_list.size()];
        string value = RandomString(4);

        int s = rand() % 3, ret;
        if (s == 0) {
            ret = kv739_put(BadStringConversion(key), BadStringConversion(value),
                    buffer);
            cout << "put " << key << ", " << value << " returned " << ret << endl;
        } else {
            ret = kv739_get(BadStringConversion(key), buffer);
            cout << "get " << key << " returned " << ret << endl;
        }
        if (ret == -1) {
            cout << "Failed TestCorrectness at 1" << endl;
            return false;
        }
        if (table.count(key) == 0) {
            if (ret == 0) {
                cout << "Failed TestCorrectness at 2" << endl;
                return false;
            }
        } else {
            if (ret == 1) {
                cout << "Failed TestCorrectness at 3" << endl;
                return false;
            }
            if (table[key] != buffer) {
                cout << "Failed TestCorrectness at 4" << endl;
                return false;
            }
        }
        if (s == 0)
            table[key] = value;
    }
    return true;
}

pair<double, double> TestPerformance() {
    vector<string> get_requests;
    vector<pair<string, string> > put_requests;
    int num_requests = 50000;

    for (int i = 0; i < num_requests; ++i)
        get_requests.push_back(RandomString(4));
    clock_t t0 = clock();
    for (int i = 0; i < num_requests; ++i)
        kv739_get(BadStringConversion(get_requests[i]), buffer);
    clock_t t = clock() - t0;
    double get_thru = (double) num_requests / ((double) t / CLOCKS_PER_SEC);

    for (int i = 0; i < num_requests; ++i)
        put_requests.push_back(make_pair(RandomString(10), RandomString(10)));
    t0 = clock();
    for (int i = 0; i < num_requests; ++i)
        kv739_put(BadStringConversion(put_requests[i].first), BadStringConversion(put_requests[i].second), buffer);
    t = clock() - t0;
    double put_thru = (double) num_requests / ((double) t / CLOCKS_PER_SEC);

    map<string, string> table;
    for (int i = 0; i < num_requests; ++i) {
        if (kv739_get(BadStringConversion(get_requests[i]), buffer) == 0)
            table[get_requests[i]] = buffer;
        if (kv739_get(BadStringConversion(put_requests[i].first), buffer) == 0)
            table[put_requests[i].first] = buffer;
    }
    int fd = open(persistence_test_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, mode644);
    for (map<string, string>::iterator iter = table.begin();
            iter != table.end();
            ++iter) {
        Message message(iter->first, iter->second);
        message.Serialize(fd);
    }
    close(fd);

    return make_pair(get_thru, put_thru);
}

void signal_handler(int) {}

int main() {
    signal(SIGPIPE, signal_handler);
    srand(time(0));
    string address = "127.0.0.1", port = "8000";
    cout << "Server address [default " << address << "]: ";
    string line;
    getline(cin, line);
    if (!line.empty())
        address = line;
    cout << "Port [default " << port << "]: ";
    getline(cin, line);
    if (!line.empty())
        port = line;
    if (kv739_init(BadStringConversion(address + ":" + port)) != 0) {
        cout << "Failed to init" << endl;
        return 1;
    }
    bool persist_ok = TestPersistence();
    if (!persist_ok)
        cout << "Persistent test failed" << endl;
    if (!TestNaive())
        return 1;
    if (!TestCorrectness())
        return 1;

    cout << "Testing throughput..." << endl;
    pair<double, double> thru = TestPerformance();
    cout << "Get throughput: " << thru.first << " req/s" << endl;
    cout << "Put throughput: " << thru.second << " req/s" << endl;
    cout << "Persistent storage test: " << (persist_ok ? "Passed" : "Failed") << endl;

    return 0;
}
