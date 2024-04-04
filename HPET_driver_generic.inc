[BITS 64]
[DEFAULT REL]



; rax - desired frequency
; rbx - interrupt vector
; rcx - timerN
; rdx - intN
init_hpet:
	cli
	push rdx
	push rbx
	push rax
	push rcx

	call get_ACPI_HPET_BAR
	cmp rax, -1
	je .error_1
	cmp rax, -2
	je .error_2
	cmp rax, -3
	je .error_3

	mov r8, [rax+44] ; save HPET MMIO BAR
	mov r9, rax ; save HPET ACPI tbl address
	mov qword [HPET], r8

;====================Check=Periodic=mode=============
	pop rcx
	shl rcx, 5 
	test qword [r8+rcx+0x100], 0x10
	jz .error_6 ; no periodic mode

;=================Frequency==========================
	mov rbx, qword [r8]
	shr rbx, 32

	mov rax, qword [.var10pow15]
	xor rdx, rdx
	div rbx ; timer frequency

	pop rbx
	xor rdx, rdx
	div rbx ; timer frequency/desired frequency = comparator value in rax

	xor rbx, rbx
	mov bx, word [r9+53]
	cmp rax, rbx
	jb .error_4 ; desired frequency not implemented

;===============Halt=HPET=set=initial=settings==================
	mov qword [r8+0x10], 0 ; disable legacy interrupt mapping, halt HPET
	btr qword [r8+rcx+0x100], 2 ; disable timer
	btr qword [r8+rcx+0x100], 1 ; set edge triggered mode
	or qword [r8+rcx+0x100], (1<<3) | (1<<6) ; periodic | some stuff bout comparators acc
	mov qword [r8+0xF0], 0 ; main counter value - 0

;===============Comparator=width=check==================
	test qword [r8+rcx+0x100], 0x20
	jnz .64_comp ; has 64 bit mode

	bsr rbx, rax
	cmp rbx, 31
	ja .error_5 ; only 32-bit mode implemented, needed comparator value > 32 bits
	mov dword [r8+rcx+0x108+4], eax ; save comparator
	jmp .end_comp

	.64_comp:
	btr qword [r8+rcx+0x100], 8 ; make sure timer is not in legacy 32-bit emulation
	mov qword [r8+rcx+0x108], rax ; save comparator
	.end_comp:
;==============IDT============================	
	xchg qword [rsp], rcx  ; ISR address, timerN saved on stack
	mov rax, qword [rsp+8] ; intN
	mov bx, cs
	xor dl, dl
	mov dh, 0x8E
	call write_IDT_gate

	pop rcx ; get timerN value back

;==============Route=interrupts=========================
	test qword [r8+rcx+0x100], 0x8000
	jz .no_FSB

	mov rax, [lapic]
	mov edx, dword [rax+0x20]
	shl edx, 12
	or rax, rdx
	bts rax, 3
	btr rax, 2
	shl rax, 32
	or rax, qword [rsp] ; high dword - "lapic" addr, low dword - INTn
	add rsp, 8
	and rax, 0xFFFFFFFFFFFF78FF ; some other configurations (vol 3 intel programming manual)
	mov qword [r8+rcx+0x110], rax 

	or qword [r8+rcx+0x100], (1<<2)|(1<<14) ; enable ints | enable FSB mapping
	or qword [r8+0x10], 1
	xor rax, rax
ret

	.no_FSB: ; timerN - rcx, intN - stack
	mov rax, qword [r8+rcx+0x100]
	shr rax, 32
	bsf rax, rax ; timerN interrupt routing
	mov rdx, rax
	shl rdx, 9
	or qword [r8+rcx+0x100], rdx

	push rcx
	call get_ioapic_id_offs
	pop rcx

	mov dword [rax], edx
	pop rdx
	bts edx, 13
	mov dword [rax+0x10], edx

	inc dword [rax]

	mov rdx, qword [lapic]
	mov edx, dword [rdx+0x20]
	shl edx, 24
	mov dword [rax+0x10], edx

	or qword [r8+rcx+0x100], (1<<2)
	or qword [r8+0x10], 1
	mov rax, 1
ret

.error_1:
.error_2:
.error_3:
add rsp, 32
ret
.error_4:
add rsp, 16
mov rax, -4
ret
.error_5:
add rsp, 16
mov rax, -5
ret
.error_6:
add rsp, 24
mov rax, -6
ret
.var10pow15 dq 1000000000000000

%include "../HPET/HPET.inc"