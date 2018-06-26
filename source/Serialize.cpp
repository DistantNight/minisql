#include "Serialize.h"

void pack(int integer, int8_t * destination)
{
    std::memcpy(destination, reinterpret_cast<void const*>(&integer), sizeof(int));
}

void pack(const char * str, size_t length, int8_t * destination)
{
    std::memcpy(destination, str, length);
}

void pack(const std::string& str, int8_t * destination)
{
    memcpy(destination, str.c_str(), str.length());
}

void pack(float decimal, int8_t * destination)
{
    std::memcpy(destination, reinterpret_cast<void const*>(&decimal), sizeof(float));
}

int unpackInt(const int8_t * source)
{
    int value;
    memcpy(&value, source, sizeof(int));
    return value;
}

float unpackFloat(const int8_t * source)
{
    float value;
    memcpy(&value, source, sizeof(float));
    return value;
}

std::string unpackString(const int8_t * source, size_t length)
{
    auto *str = new char[length + 1];
    memcpy(str, source, length);
    str[length] = '\0';
    std::string s(str);
    delete[] str;
    return s;
}
