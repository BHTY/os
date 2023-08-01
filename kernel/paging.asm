extern enable_paging
extern set_page_tables

enable_paging:
	mov eax, cr0
	or eax, 0x80000001
	mov cr0, eax
	ret
	
set_page_tables:
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]
	mov cr3, eax
	pop ebp
	ret
