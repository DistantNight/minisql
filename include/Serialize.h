#pragma once
#include <cstdint>
#include <string>
#include <sstream>

void pack(int integer, int8_t *destination);
void pack(const char *str, size_t length, int8_t *destination);
void pack(const std::string& str, int8_t *destination);
void pack(float decimal, int8_t *destination);

int unpackInt(const int8_t *source);
float unpackFloat(const int8_t *source);
std::string unpackString(const int8_t *source, size_t length);