[org 0x7C00]

; store # of boot disk

mov ax, 0
mov es, ax
mov [diskNum], dl

; read drive geometry

mov ah, 8
int 0x13 ; DH = # of heads - 1, CL & 0x3F = sectors per track

jmp $

loadsector: ;this function reads a sector off of the disk
	mov BP, SP
	mov al, BYTE [BP+0x02] ; # of sectors to read
	mov cx, WORD [BP+0x04] ; cylinder # stored in ch, sector # stored in cl
	mov dh, BYTE [BP+0x06] ; head #
	mov bx, WORD [BP+0x08] ; address to store into
	mov ah, 2
	mov dl, [diskNum]
	int 0x13
	ret

; USTAR_SEEK scans the disk (starting from sector 2) to find the file
; with the name pointed to by si
; it returns the sector on which it is stored and the # of sectors of data

ustar_seek:
	ret

diskNum: db 0
filename: db "kernel.bin", 0

times 510-($-$$) db 0
db 0x55, 0xaa

KERNEL_LOCATION equ 0x1000
