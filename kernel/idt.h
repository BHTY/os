#include <stdint.h>

#define IDT_MAX_DESCRIPTORS 256

typedef struct{
	uint16_t isr_low;
	uint16_t kernel_cs;
	uint8_t reserved;
	uint8_t flags;
	uint16_t isr_high;
} __attribute__((packed)) idt_entry_t;

typedef struct{
	uint16_t limit;
	uint32_t base;
} __attribute__((packed)) idtr_t;

extern void* isr_stub_table[];

__attribute__((noreturn)) void exception_handler(void);
void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags);
void idt_init(void);
