[BITS 64]
[DEFAULT REL]

%include "../jd9999_hdr_macro.inc"
jd9999_hdr_macro textsize, datasize, 0x0, textsize+datasize+1024

%macro print 1
	mov rcx, qword [EFI_SYSTEM_TABLE]
	mov rcx, qword [rcx+64]
	lea rdx, [%1]
	call [efi_print]
%endmacro

%macro getch 1
	mov rcx, qword [EFI_SYSTEM_TABLE]
	mov rcx, qword [rcx+48]
	lea rdx, [%1]
	call [efi_getch]
%endmacro


section .text follows=.header
start:
	sub rsp, 6*8+8 ; Copied from Charles AP's implementation, fix stack alignment issue (Thanks Charles AP!)

	mov qword [EFI_HANDLE], rcx
	mov qword [EFI_SYSTEM_TABLE], rdx

	mov rax, qword [EFI_SYSTEM_TABLE]
	mov rax, qword [rax+64]
	mov rax, qword [rax+8]
	mov qword [efi_print], rax

	mov rax, qword [EFI_SYSTEM_TABLE]
	mov rax, qword [rax+48]
	mov rax, qword [rax+8]
	mov qword [efi_getch], rax

	print gotchastr

	call get_MADT_BAR
	call conf_LAPIC
	call disable_PIC

	print gotchastr

	cli
	mov rax, 1000
	lea rbx, [intfunc]
	xor rcx, rcx
	mov rdx, 0x22
	call init_hpet
	sti

	lea rdi, [tststr+34]
	call cvt2hex
	print tststr

	;int 0x22
;117333
	mov rax, qword [HPET]
	mov rax, qword [rax+0x100]
	lea rdi, [tststr+34]
	mov rax, qword [counter]
	call cvt2hex
	print tststr


	mov rax, 3000
	call sleep

	mov rax, qword [counter]
	lea rdi, [tststr+34]
	call cvt2hex
	print tststr

; before - 0x0000000400000030
; after  - 0x000000040000043C

	cli
	hlt

	; rax - amount of miliseconds
	sleep:
		add rax, qword [counter]
		._loop:
			pause
			cmp rax, qword [counter]
		jnz ._loop
	ret

	intfunc:
		inc qword [counter]
		push rax
		mov rax, 0xFEE000B0
		mov dword [rax], 0
		pop rax
	iretq


	%include "HPET_driver_generic.inc"
	%include "../MADT/MADT.inc"
	%include "../ioapic/ioapic.inc"
	%include "../apic/apic.inc"
	%include "../IDT/IDT.inc"	
	%include "../cvt2hex/cvt2hex.inc"


times 2048 - ($-$$) db 0 ;alignment
textsize equ $-$$

section .data follows=.text
dataStart:
	EFI_HANDLE dq 0
	EFI_SYSTEM_TABLE dq 0

	efi_print dq 0
	efi_getch dq 0

	counter dq 0

	tststr dw  __utf16__(`0x0000000000000000\r\n\0`)
	gotchastr dw __utf16__(`start!\r\n\0`)
	intstr dw  __utf16__(`INT!\r\n\0`)

	HPET_acpi dq 0
	HPET_BAR  dq 0

	key:
		.sccode dw 0
		.char dw 0

	MADT dq 0
	XSDT dq 0
	MCFG dq 0
	RSDP dq 0

	io_apic dq 0
	io_apic_index dd 0

	lapic dq 0

	HPET dq 0

times 2048 - ($-$$) db 0 ;alignment
datasize equ $-$$
