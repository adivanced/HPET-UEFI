[BITS 64]
[DEFAULT REL]

; No args (implicit - need global variables EFI_SYSTEM_TABLE, RSDP, MADT, XSDT)
; return values - rax - MADT address
; return upon error - 1 if could not locate RSDP, 2 if could not get XSDT, 3 if could not locate MADT
align 8
get_MADT_BAR:  
	cmp qword [RSDP], 0
	jne .have_RSDP
	.find_rsdp:
		mov rax, qword [EFI_SYSTEM_TABLE]
		mov rdi, qword [rax+112]
		mov rdx, qword [rax+104]

		xor rax, rax
		cld

		.findloop_RSDP:
			lea rsi, [.RSDP_GUID_analytic_2]
			mov rcx, 16
			repe cmpsb
			jz .equal

			lea rdi, [rdi+rcx+8]
			inc rax
			cmp rax, rdx
		jnz .findloop_RSDP

		jmp .end_1
		.equal:
			mov rdi, qword [rdi]
			mov qword [RSDP], rdi
	.have_RSDP:

	cmp qword [XSDT], 0
	jne .have_XSDT

	mov rcx, qword [RSDP]
	mov rcx, qword [rcx+24]
	mov qword [XSDT], rcx
	cmp dword [rcx], 'TDSX'
	jz .end_2

	.have_XSDT:

	cmp qword [MADT], 0
	jne .have_MADT

	.find_MADT:
		mov rdi, qword [XSDT]
		mov edx, dword [rdi+4] ; xsdt size
		sub edx, 36 
		shr edx, 3 ; number of entries
		add rdi, 36

		mov ecx, 'APIC'				
		xor rax, rax

		.findloop_MADT:
			mov rsi, qword [rdi+rax*8]	
			; mov ebx, dword [rsi]
			; mov dword [array+rax*4], ebx
			cmp ecx, dword [rsi]
			jz .equal_MADT

			;add rdi, 8
			inc rax
			cmp rax, rdx
		jnz .findloop_MADT

	jmp .end_3
	.equal_MADT:
		mov qword [MADT], rsi
		.have_MADT:
		mov rax, qword [MADT]
ret
	
	.end_3:
	mov rax, 3
ret
	.end_2:
	mov rax, 2
ret
	.end_1:
	mov rax, 1
ret
.RSDP_GUID_analytic_2 db 0x71, 0xe8, 0x68, 0x88, 0xf1, 0xe4, 0xd3, 0x11, 0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81


; returns the LAPIC id the MADT table in al, ACPI cpu id in ah
get_LAPIC_ids:
	mov rbx, qword [MADT]
	mov edx, dword [rbx+4]
	add rbx, 0x2c
	sub edx, 0x2c

	xor rcx, rcx
	._loop:
		cmp byte [rbx], 0
		jz .equal
		mov cl, byte [rbx+1]
		add rbx, rcx
		sub edx, ecx
	jnz ._loop
	mov rax, -1
ret
.equal:
	xor rax, rax
	mov al, byte [rbx+2]
	mov ah, byte [rbx+3]
ret