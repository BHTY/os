#pragma once
#include <stdint.h>

typedef struct{
	uint32_t entries[1024];
} __attribute__((aligned(4096))) page_directory;

typedef struct{
	uint32_t entries[1024];
} __attribute__((aligned(4096))) page_table;

void init_paging();

void enable_paging();
void set_page_tables(uint32_t);

//allocates and frees pages in physical memory
void* allocate_page();
void free_page(void*);

//maps physical addresses to virtual addresses
int mmap(page_directory* dir, void* vaddr, void* paddr);
void unmap();

void identity_map(page_directory* dir);
