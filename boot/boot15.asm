[org 0x7e00]

cli
lgdt [GDT_Descriptor]
mov eax, cr0
or eax, 1
mov cr0, eax

jmp CODE_SEG:start_protected_mode

jmp $

CODE_SEG equ code_descriptor - GDT_Start
DATA_SEG equ data_descriptor - GDT_Start

GDT_Start:
	null_descriptor:
		dd 0
		dd 0
	code_descriptor:
		dw 0xffff
		dw 0
		db 0
		db 0x9a
		db 0xcf
		db 0
	data_descriptor:
		dw 0xffff
		dw 0
		db 0
		db 0x92
		db 0xcf
		db 0
	GDT_End:

GDT_Descriptor:
	dw GDT_End - GDT_Start - 1 ;size
	dd GDT_Start ;start

[bits 32]
start_protected_mode:
	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ebp, 0x90000
	mov esp, ebp

;	mov esi, KERNEL_LOCATION
;	mov ecx, 512
;	mov edi, 0xb8000
;	rep movsb

;	jmp $
	jmp CODE_SEG:KERNEL_LOCATION

times 512-($-$$) db 0

KERNEL_LOCATION equ 0x1000
