#include "term.h"
#include <stddef.h>
#include <stdint.h>
#include "../crt/string.h"
#include "../crt/printf.h"
#include "../io.h"

#define TEXT_BUFFER (char*)0xB8000
#define CHARS_PER_LINE 80
#define LINES 25

uint16_t cursor_position = 0;
uint8_t COLOR_ATTR = 0x20;

void gotoxy(uint8_t x, uint8_t y){
	cursor_position = CHARS_PER_LINE * y + x;
    //set cursor position
    io_write_8(0x3D4, 14);
    io_write_8(0x3D5, cursor_position >> 8);
    io_write_8(0x3D4, 15);
    io_write_8(0x3D5, cursor_position);
}

uint16_t getpos(){
	return cursor_position;
}

void setcolor(uint8_t col){
    COLOR_ATTR = col;
}

void scroll(){
    for(int i = 0; i < LINES - 1; i++){
        memcpy(TEXT_BUFFER + CHARS_PER_LINE*i*2, TEXT_BUFFER + CHARS_PER_LINE*(i+1)*2, CHARS_PER_LINE * 2);    
    }

    memset(TEXT_BUFFER + (LINES-1)*CHARS_PER_LINE*2, 0, CHARS_PER_LINE * 2);
}

void kputch(char ch){
    if(ch == 0x08 && cursor_position > 0){ //backspace
        cursor_position--;
    }

    else if(ch == 0x09){
        cursor_position++;
    }

    else if(ch == 0x0b){ //down one row
        cursor_position += CHARS_PER_LINE;
    }

    else if(ch == 0x0d){ //carriage return
        cursor_position = (cursor_position - (cursor_position % CHARS_PER_LINE));
    }

    else if(ch == 0x0a){ //\n
        cursor_position = 80 + (cursor_position - (cursor_position % CHARS_PER_LINE));
    }

    else{ //standard character
        *(TEXT_BUFFER + cursor_position * 2) = ch;
        *(TEXT_BUFFER + cursor_position * 2 + 1) = COLOR_ATTR;

        cursor_position++;
    }

    if(cursor_position >= (CHARS_PER_LINE * LINES)){
        scroll();
        cursor_position -= CHARS_PER_LINE;
    }

    //set cursor position
    io_write_8(0x3D4, 14);
    io_write_8(0x3D5, cursor_position >> 8);
    io_write_8(0x3D4, 15);
    io_write_8(0x3D5, cursor_position);
}

void kputs(const char* str){
    while(*str){
        kputch(*(str++));
    }
}

int kprintf(const char* fmt, ...){
	char buf[1024];
	va_list args;
	int i;
	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	buf[i] = '\0';
	kputs(buf);
	return i;
}
