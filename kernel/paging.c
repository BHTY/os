#include <stdint.h>
#include "crt/string.h"
#include "paging.h"

#define MEMORY_BASE 0x100000
#define MEMORY_SIZE 0x100000
#define NUM_PAGES MEMORY_SIZE/4096

uint8_t free_pages[NUM_PAGES];

void init_paging(){
	memset(free_pages, 0, NUM_PAGES);
	memset(MEMORY_BASE, 0, MEMORY_SIZE);
}

void* alloc_page(){
	for(int i = 0; i < NUM_PAGES; i++){
		if(free_pages[i] == 0){
			free_pages[i] = 1;
			return i * 4096 + MEMORY_BASE;
		}
	}
	
	return 0;
}

void free_page(void* page){
	free_pages[(uint32_t)(page - MEMORY_BASE) >> 12] = 0;
}

//possible error codes include
//0.) Success
//1.) Virtual address occupied
//2.) Alignment error
int mmap(page_directory* dir, void* vaddr, void* paddr){
	uint32_t pd_entry = (uint32_t)(vaddr) >> 22; //entry into page directory
	uint32_t pt_entry = (uint32_t)(vaddr) >> 12 & 0x3FF; //entry into pagetable
	
	if(dir->entries[pd_entry] == 0){ //the virtual address points to an invalid pagetable
		dir->entries[pd_entry] = (uint32_t)(alloc_page()) | 7;
	}
	
	page_table* table = dir->entries[pd_entry] & 0xfffff000;
	table->entries[pt_entry] = (uint32_t)(paddr) | 7;
}

void identity_map(page_directory* dir){
	uint32_t base_address = 0;
	
	for(int i = 0; i < 256; i++){
		mmap(dir, base_address, base_address);
		base_address += 4096;
	}
}
