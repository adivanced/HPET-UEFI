[BITS 64]
[DEFAULT REL]


; return value in rax - HPET table BAR
get_ACPI_HPET_BAR:
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
			;mov qword [RSDP], rdi

	;mov rdi, qword [RSDP]
	mov rdi, qword [rdi+24]
	cmp dword [rdi], 'XSDT'
	jnz .end_2

	;mov qword [XSDT], rdi

	.find_HPET:
		mov edx, dword [rdi+4]
		sub edx, 36
		shr edx, 3
		add rdi, 36

		mov ecx, 'HPET'
		xor rax, rax

		.findloop_HPET:
			mov rsi, qword [rdi+rax*8]
			cmp ecx, dword [rsi]
			jz .equal_HPET

			inc rax
			cmp rax, rdx
		jnz .findloop_HPET

		jmp .end_3

		.equal_HPET:
			mov rax, rsi
ret
	.end_3:
	mov rax, -3
ret
	.end_2:
	mov rax, -2
ret
	.end_1:
	mov rax, -1
ret
.RSDP_GUID_analytic_2 db 0x71, 0xe8, 0x68, 0x88, 0xf1, 0xe4, 0xd3, 0x11, 0xbc, 0x22, 0x00, 0x80, 0xc7, 0x3c, 0x88, 0x81




