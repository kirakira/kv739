#ifndef DATABASE_H_
#define DATABASE_H_

#include <string>
#include <map>

void database_init(const std::string& file, std::map<std::string, std::string>* table);
void database_put(const std::string& file, const std::string& key, const std::string& value);

#endif  // DATABASE_H_
