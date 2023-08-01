//#include <string.h>
#include <stdint.h>
#include <stddef.h>

void* memset(void* ptr, int value, size_t num){
	uint8_t* pointer = ptr;

	while(num--){
		*(pointer++) = value;
	}

	return ptr;
}

void* memcpy(void* dst, const void* src, size_t num){
	uint8_t* ptr = dst;
	uint8_t* source = src;
	
	while(num--){
		*(ptr++) = *(source++);
	}	
	
	return dst;
}

int strlen(const char* str){
	int i = 0;
	
	while(*(str++)){
		i++;
	}
	
	return i;
}
