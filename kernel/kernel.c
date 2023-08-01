/*
PC Operating System/3 Version 0.01
Written by Will Klees (c) 2023
*/

#include <stdint.h>
#include "crt/string.h"
#include "idt.h" 
#include "paging.h"

page_directory* pagedir;

void kmain(){
	pagedir = allocate_page();

	init_paging();
	identity_map(pagedir);
	set_page_tables(pagedir);
	enable_paging();
	
	while(1){
		*(unsigned char*)(0xB8000) = *(unsigned char*)(0xB8000)+1;
		//*(unsigned char*)(0x100001) = *(unsigned char*)(0x100001)+1;
	}
	
	while(1);
}