[org 0x7C00]

; store # of boot disk

xor ax, ax
mov es, ax
mov ax, cs
mov ds, ax
mov [diskNum], dl

; read drive geometry

mov ah, 8
int 0x13 ; DH = # of heads - 1, CL & 0x3F = sectors per track
inc dh
mov [number_heads], dh
and cl, 0x3f
mov [sectors_per_track], cl

; find the kernel image
mov si, filename
call ustar_seek

; load the kernel image
push bx
push ax
call loadsector_lba

; jump into the stage 1.5 bootloader (i.e. pmode entry stub) to reach protected mode
push 0x7E00
push 0x00
push 0x0002
push 0x01

call loadsector

jmp 0x0000:0x7E00

loadsector_lba: ;this function takes an LBA address off the stack, converts it to CHS, then loads it
	mov BP, SP

	mov ax, WORD [BP+0x02]
	mov dx, 0
	div WORD [sectors_per_track] ;quotient(AX)=temp value, rem(DX)=sector-1
	mov BX, DX ;moving sector # - 1 into BX
	inc BX ;BX contains sector #

	div WORD [number_heads] ; quotient(AX)=cylinder #, rem(DX)=head #	

	mov cl, bl ;sector #
	mov ch, al ;cylinder #

	push KERNEL_LOCATION ;address to store into
	push dx ;push head # onto the stack
	push cx ;push cylinder & sector #
	push WORD [BP+0x04] ; push # of sectors to read

	call loadsector

	add SP, 8
	ret

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
; with the name pointed to by SI
; Return values
; AX = Sector #
; BX = # of sectors

ustar_seek:
	mov ax, 2

	ustar_seek_loop:
		; load sector
		push ax ;push it once to save it
		push 0x01 ;only reading 1 sector
		push ax ;push it again as an arg
		call loadsector_lba

		add SP, 4
		pop ax

		; determine filesize
		call get_size
		
		; compare to filename we're seeking for
			push si ;push it once to save
			push WORD [filename_len]
			push KERNEL_LOCATION
			push si
			call memcmp
			add SP, 6
			pop si
		; 	if the names are equal, return
		cmp cl, 1
		je ustar_seek_exit

		; increment
		add ax, bx

		jmp ustar_seek_loop
		
	ustar_seek_exit:
		add ax, 1
		ret

; BP+2=ptr1 (DS:SI) - this is the one from this sector
; BP+4=ptr2 (ES:DI) - this is the one from the sector we loaded in
; BP+6=length
; Result in CL

memcmp:
	mov BP, SP

	mov dx, ax
	mov di, WORD [BP+0x04]
	mov cx, WORD [BP+0x06]
memcmp_loop:
	mov ah, 0x0e
	mov al, BYTE [es:di]
	int 0x10
	inc di
	dec cx
	cmp cx, 0
	jne memcmp_loop

	mov ah, 0x0e
	mov al, 0xa
	int 0x10
	mov ah, 0x0e
	mov al, 0xd
	int 0x10

	mov ax, dx

	mov si, WORD [BP+0x02]
	mov di, WORD [BP+0x04]
	mov cx, WORD [BP+0x06]
	repe cmpsb
	sete cl
	ret

; This function reads the currently-loaded disk fragment from KERNEL_LOCATION
; If called correctly, it's the header of a USTAR file
; Returns size (in sectors) in BX
; The data is located at ES:107C and is 12 bytes long

get_size:
	mov cl, 12
	mov di, 0x107c
	xor bx, bx
	xor dx, dx

	get_size_loop:
		shl bx, 3
		mov dl, BYTE [es:di]
		sub dl, 48
		add bx, dx
		inc di
		dec cl
		cmp cl, 0
		jne get_size_loop

	get_size_done:
		mov cx, bx
		and cx, 0x1FF
		shr bx, 9
		cmp cx, 0
		je get_size_exit
	get_size_add_one:
		inc bx

	get_size_exit:
		ret

number_heads: dw 0
sectors_per_track: dw 0
diskNum: db 0
filename: db "kernel.bin"
filename_len: dw 10

times 510-($-$$) db 0
db 0x55, 0xaa

KERNEL_LOCATION equ 0x1000
