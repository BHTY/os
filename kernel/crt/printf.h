#pragma once
#include <stdarg.h>
#include <stdint.h>

int vsprintf(char*, const char*, va_list);
int sprintf(char*, const char*, ...);
