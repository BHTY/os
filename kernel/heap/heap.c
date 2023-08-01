#include <stddef.h>
#include "../crt/string.h"
#include "../paging.h"

#define align(x, a) (((x % a) == 0) ? x : (x + a) - x % a)

extern page_directory pagedir;
void* heapStart = 0x800000;
size_t heapSize = 0;

typedef struct MemBlock{
	uint8_t used;
	size_t size;
	struct MemBlock *prev;
	struct MemBlock *next;
	uint8_t data[0];
} MemBlock;

void init_heap(){
	MemBlock temp;
	temp.used = 0;
	temp.size = 4096 - sizeof(MemBlock);
	temp.prev = 0;
	temp.next = 0;	
	heapSize = 4096;
	mmap(&pagedir, heapStart, allocate_page());
	memcpy(heapStart, &temp, sizeof(MemBlock));
}

void *malloc(size_t sz){
	MemBlock *curBlock = (MemBlock*)heapStart;
	MemBlock *oldNext;
	MemBlock *nextBlock;
	size_t oldSize;

	sz = align(sz, 4);

	while (1){

		if (!(curBlock->used)){ //free block found

			if (curBlock->size >= (16 + sz)){ //is block big enough?
				oldSize = curBlock->size;
				oldNext = curBlock->next;
				//format current block
				curBlock->used = 1;
				curBlock->size = sz;
				curBlock->next = (MemBlock*)((size_t)curBlock + 16 + sz);
				nextBlock = curBlock->next;

				nextBlock->used = 0;
				nextBlock->prev = curBlock;

				if (oldNext == 0 || oldNext->used){ 
					nextBlock->next = oldNext;
					nextBlock->size = oldSize - (16 + sz);
				}
				else{ //oldnext is free, merge
					nextBlock->next = oldNext->next;
					nextBlock->size = oldSize - sz + oldNext->size;
				}

				break;
			}
			
			else if(curBlock->next == 0){ //but it is the last block so it's the best we've got
				int needed_bytes = sz + 32 - curBlock->size; //this is the # of bytes by which we need to extend the heap
				needed_bytes = align(needed_bytes, 4096);
				int needed_pages = needed_bytes / 4096;
				
				//grow the heap
				for(int i = 0; i < needed_pages; i++){
					mmap(&pagedir, heapStart + heapSize, allocate_page());
					heapSize += 4096;
				}
				
				curBlock->used = 1;
				curBlock->size = sz;
				curBlock->next = (MemBlock*)((size_t)curBlock + 16 + sz);
				nextBlock = curBlock->next;
				nextBlock->used = 0;
				nextBlock->prev = curBlock;
				nextBlock->size = needed_bytes; //???
				nextBlock->next = 0;
				
				break;				
			}

		}

		curBlock = curBlock->next;

        if(!curBlock){
            return NULL;
        }
	}

	return curBlock->data;
}

void free(void* ptr){
	MemBlock* curBlock = (MemBlock*)((size_t)ptr - 16);
	//printf("%p", curBlock);

	if (curBlock->prev && !(curBlock->prev->used)){ //if block behind this is free, merge
		curBlock->prev->next = curBlock->next;
		curBlock->prev->size += 16 + curBlock->size;
		curBlock = curBlock->prev;
	}

	//if next block is unused, merge with that as well
	if (curBlock->next && !(curBlock->next->used)){
		curBlock->size += 16 + curBlock->next->size;
		curBlock->next = curBlock->next->next;
	}

	//mark as free
	curBlock->used = 0;
}
