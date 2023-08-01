#pragma once
#include <stddef.h>

void* memset(void*, int, size_t);
void* memcpy(void*, const void*, size_t);
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
int memcmp(const void*, const void*, size_t);
int strlen(const char*);
