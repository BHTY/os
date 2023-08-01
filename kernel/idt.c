#include <stdint.h>
#include "idt.h"

__attribute__((aligned(0x10))) static idt_entry_t idt[256];

static idtr_t idtr;

#define io_write_8(port, data) __asm__ volatile("outb %1, %0" : : "dN" ((uint16_t)port), "a" ((uint8_t)(data)))

__attribute__((noreturn)) void exception_handler(int code){
	if(code != 8){

	*(unsigned char*)(0xB8002) = code + 48;
	*(unsigned char*)(0xB8003) = *(unsigned char*)(0xB8003) + 1;
	io_write_8(0xA0, 0x20);
	io_write_8(0x20, 0x20);
	}
	
	//__asm__ volatile("cli; hlt");
	//__asm__ volatile("cli");
	//*(unsigned char*)(0xB8000) = *(unsigned char*)(0xB8000) + 1;
	//__asm__ volatile("sti");
}

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    idt_entry_t* descriptor = &idt[vector];
 
    descriptor->isr_low        = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs      = 0x08; // this value can be whatever offset your kernel code selector is in your GDT
    descriptor->flags          = flags;
    descriptor->isr_high       = (uint32_t)isr >> 16;
    descriptor->reserved       = 0;
}

void idt_init() {
	idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(idt_entry_t) * IDT_MAX_DESCRIPTORS - 1;
 
    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
        //vectors[vector] = 1;
    }
 
    __asm__ volatile ("lidt %0" : : "m"(idtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}
