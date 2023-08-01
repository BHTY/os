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
