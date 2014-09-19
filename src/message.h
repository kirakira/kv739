#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include "common.h"

class Message {
    private:
        bool contains_value_;
        std::string key_;
        std::string value_;

    public:
        Message()
            : contains_value_(false) {}

        explicit Message(const std::string& key)
            : contains_value_(false),
            key_(key) {}

        Message(const std::string& key, const std::string& value)
            : contains_value_(true),
            key_(key), value_(value) {}

        bool Deserialize(int fd) {
            if (ReadN(fd, (char*) &contains_value_, sizeof(contains_value_)) <= 0)
                return false;
            if (!ReadString(fd, &key_))
                return false;
            if (contains_value_ && !ReadString(fd, &value_))
                return false;
            return true;
        }

        bool ContainsValue() const {
            return contains_value_;
        }

        std::string key() const {
            return key_;
        }

        std::string value() const {
            return value_;
        }

        bool Serialize(int fd) const {
            if (WriteN(fd, (const char*) &contains_value_, sizeof(contains_value_)) <= 0)
                return false;
            if (!WriteString(fd, key_))
                return false;
            if (contains_value_ && !WriteString(fd, value_))
                return false;
            return true;
        }
};

#endif  // MESSAGE_H
