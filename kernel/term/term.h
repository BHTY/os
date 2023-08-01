#pragma once
#include <stdint.h>

void gotoxy(uint8_t x, uint8_t y);
uint16_t getpos();
void setcolor(uint8_t);
void kputch(char);
void kputs(const char*);
int kprintf(const char*, ...);
