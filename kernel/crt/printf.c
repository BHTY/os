#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "printf.h"
#include "string.h"	

uint32_t ilog(uint32_t num, uint32_t base){
    uint32_t t = 0;

    while(num >= 1){
        num /= base;
        t++;
    }

    return t;
}

int32_t ipow(uint32_t base, uint32_t power){
    int32_t t = 1;

    while(power > 0){
        power--;
        t *= base;
    }

    return t;
}

char hexch(uint8_t num){ //for a number between 0 and 15, get the hex digit 0-f
    if(num < 10){
        return (char)(num + 48);
    }
    return (char)(num + 55);
}

int32_t hex(uint32_t num, char* str){ //places hex of number into str
    str[0] = '0';
    str[1] = 'x';

    //determine number of places and move backwards
    uint32_t places = ilog(num, 16);
    uint32_t i = 2;

    while(places > 1){
        uint32_t temp = ipow(16, places - 1);
        uint32_t tmp = (num - (num % temp));
        str[i] = hexch(tmp/temp);
        num -= tmp;

        places--;
        i++;
    }

    str[i] = hexch(num);
    str[i+1] = 0;

    return i+1;
}

int32_t numtostr(uint8_t *str, int num, int base, int sign){ //0=unsigned, 1=signed
	uint32_t i;
	
	if (sign && (num < 0)){
		str[0] = '-';
		i = 1;
		num = ~num + 1;
	}
	else{
		i = 0;
	}

	uint32_t places = ilog(num, base);

	while (places > 1){
		uint32_t temp = ipow(base, places - 1);
		uint32_t tmp = (num - (num % temp));
		str[i] = hexch(tmp / temp);
		num -= tmp;

		places--;
		i++;
	}

	str[i] = hexch(num);
	str[i + 1] = 0;

	return i + 1;
}

int vsprintf(char *buf, const char *fmt, va_list args){

	uint8_t *str;
	uint16_t i;
	uint16_t len;
	uint8_t *s;

	for (str = buf; *fmt; ++fmt){

		if (*fmt != '%'){
			*str++ = *fmt;
			continue;
		}

		fmt++;

		switch (*fmt){
			case 'c':
				*str++ = (uint8_t)va_arg(args, int);
				break;
			case 's':
				s = va_arg(args, uint8_t*);
				len = strlen(s);
				for (i = 0; i < len; ++i) *str++ = *s++;
				break;
			case 'o':
				break;
			case 'x':
				str += numtostr(str, va_arg(args, size_t), 16, 0);
				break;
			case 'X':
				str += numtostr(str, va_arg(args, size_t), 16, 1);
				break;
			case 'p':
				str += numtostr(str, va_arg(args, size_t), 16, 0);
				break;
			case 'd':
			case 'i':
				str += numtostr(str, va_arg(args, size_t), 10, 1);
				break;
			case 'u':
				str += numtostr(str, va_arg(args, size_t), 10, 0);
				break;
			case 'b':
				str += numtostr(str, va_arg(args, size_t), 2, 0);
				break;
			case 'B':
				str += numtostr(str, va_arg(args, size_t), 2, 1);
				break;
			default:
                *str++ = '%';
				break;
		}

	}

	*str = 0;

	return (uint32_t)str - (uint32_t)buf;

}

int sprintf(char* buf, const char* fmt, ...){
	va_list args;
	int i;
	va_start(args, fmt);
	i = vsprintf(buf, fmt, args);
	va_end(args);
	return i;
}
