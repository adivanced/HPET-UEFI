[BITS 64]
[DEFAULT REL]

%define IOREGSEL 0x0
%define IOWIN 0x10   ; APIC_BAR + REG_OFFS

%define IOAPICID 0x0
%define IOAPICVER 0x1
%define IOAPICARB 0x2
%define IO_IRQn_LOW(x) 0x10+(x)*2
%define IO_IRQn_HIGH(x) 0x10+(x)*2 +1


; no explicit args.
; requires MADT global variable.
; returns the amount of ioapics present on the computer. 
; the ID you may further need can be deduced from the amount of apics. 
get_ioapic_number:
	mov rbx, qword [MADT]
	mov edx, dword [rbx+4] ; length
	add rbx, 0x2c
	sub edx, 0x2c

	xor rax, rax
	xor rcx, rcx
	._loop:
		cmp byte [rbx], 1
		jz .equal
		.e_equal:
		mov cl, byte [rbx+1]
		add rbx, rcx
		sub edx, ecx
	jnz ._loop
ret
.equal:
	inc rax
jmp .e_equal

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

; rax - ioapic id.
; requires MADT global variable.
; returns the I/O apic BAR corellating with the requested ID in rax. 
get_ioapic_BAR:
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
	;xor rax, rax
	mov rax, -1
ret
.equal:
	cmp byte [rbx+2], al
	jnz .e_equal
	mov eax, dword [rbx+4]
ret

; rax - IRQn number as of PIC mapping
; requires MADT global variable.
; returns the value, IRQn is remapped to in rax, the APIC ID in rdx
; on error returns -1
get_IRQn_redirect:
	mov rbx, qword [MADT]
	mov edx, dword [rbx+4]
	add rbx, 0x2c
	sub edx, 0x2c

	xor rcx, rcx
	._loop:
		cmp byte [rbx], 2
		jz .equal
		.e_equal:
		mov cl, byte [rbx+1]
		add rbx, rcx
		sub edx, ecx
	jnz ._loop
	mov rax, -1
ret
.equal:
	cmp byte [rbx+3], al
	jnz .e_equal
	xor rax, rax
	mov eax, dword [rbx+4]
	xor rdx, rdx
	mov dl, byte [rbx+2]
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