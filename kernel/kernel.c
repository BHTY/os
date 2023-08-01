/*
PC Operating System/3 Version 0.01
Written by Will Klees (c) 2023
*/

#include <stdint.h>
#include "term/term.h"
#include "crt/string.h"
#include "idt.h" 
#include "paging.h"

page_directory pagedir;
int year = 2023;

void test();

void map_next_megabyte(page_directory* dir){
	uint32_t base_address = 1048576;
	
	for(int i = 0; i < 256; i++){
		mmap(dir, base_address, base_address);
		base_address += 4096;
	}
}

void kmain(){
	/*for(int i = 0; i < 1024; i++){
		pagedir.entries[i] = 0;
	}*/

	//memset(&pagedir, 0, sizeof(page_directory));
	init_paging();
	identity_map(&pagedir);
	//map_next_megabyte(&pagedir);
	mmap(&pagedir, 0x200000, 0xB8000);
	set_page_tables(&pagedir);
	enable_paging();
	
	idt_init();
	
	//while(1);
	
	//init_heap();
	
	while(1){
		*(unsigned char*)(0xB8000) = *(unsigned char*)(0xB8000)+1;
		*(unsigned char*)(0x200001) = *(unsigned char*)(0x200001)+1;
	}
}
