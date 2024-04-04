conf_LAPIC:
	mov rcx, 0x1B
	rdmsr
	or eax, 100000000000b
	;push rax
	wrmsr
	shl rdx, 32
	and eax, 0xFFFFF000
	or rax, rdx ; rax - LAPIC BAR, rbx - timN routing
	
	mov qword [lapic], rax

	; mov dword [rax+0x80], 0x20     ; TPR to inhibit softint delivery
	; mov dword [rax+0x320], 0x10000 ; disable internal timer
	; mov dword [rax+0x370], 0x10000 ; disable error interrupts
	;mov dword [rax+0xF0], 0x0110F  ; enable APIC and set spurious vector to F 0x10F?

	mov dword [rax+0x350], 0x08700 ; enable LINT0 for external
	mov dword [rax+0x360], 0x00400 ; enable LINT1 for NMI
	mov dword [rax+0x080], 0 ; TPR
	mov dword [rax+0x0E0], 0xFFFFFFFF

	; rdmsr
	; pop rax
	; wrmsr
ret