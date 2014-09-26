#ifndef DATABASE_H_
#define DATABASE_H_

#include <map>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>

const mode_t mode644 = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_ISUID | S_ISGID;

void database_init(const std::string& file, std::map<std::string, std::string>* table);
bool database_get(const std::string& file, const std::string& key, std::string *value);
void database_put(const std::string& file, const std::string& key, const std::string& value);

#endif  // DATABASE_H_
