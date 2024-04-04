[BITS 64]
[DEFAULT REL]

; rax - the IRQn
; returns IOAPIC BAR in rax, IOREDTBL offset in rdx
get_ioapic_id_offs:
	;push rbx
	mov rbx, qword [MADT]
	mov edx, dword [rbx+4]
	add rbx, 0x2c
	sub edx, 0x2c

	xor rcx, rcx
	._loop:
		cmp byte [rbx], 1
		jz .equal
		.e_equal:
		mov cl, byte [rbx+1]
		add rbx, rcx
		sub edx, ecx
	jnz ._loop
	mov rax, -1
	;add rsp, 8
ret

	.equal:
		;mov edi, dword [rbx+8] ; get ints start
		cmp eax, dword [rbx+8] ; compare int start with our int
		jae .equal_2
	jmp .e_equal

	.equal_2:
		mov edi, dword [rbx+4] ; IO APIC BAR
		mov dword [rdi], 0x01 ; select IOAPIC Version register
		mov esi, dword [rdi+0x010] ; get value of IOAPIC Version register
		shr esi, 16
		and esi, 0xFF
		add esi, dword [rbx+8] ; get the the high int id of the corresponding IO APIC
		cmp eax, esi
	jae .e_equal

	;pop rdx
	mov edx, eax
	sub edx, dword [rbx+8]
	shl edx, 1
	add edx, 0x010 ; get the IOREDTBL offset 
	mov eax, edi
ret

disable_PIC:
	mov al, 0x70
	out 0x22, al
	mov al, 0x01
	out 0x23, al
	mov al, 0xFF
	out 0x21, al
	out 0xA1, al
ret
