[BITS 64]
[DEFAULT REL]

; ax - Gate №
; bx - selector
; dl - ist
; dh - type_attributes
; rcx - ISR address

; return value:
; 0  - on success
; -1 - on failure
write_IDT_gate:
	cld
	sub rsp, 10
	sidt [rsp]

	shr word [rsp], 4
	cmp ax, word [rsp]
	ja .index_error

	mov rdi, qword [rsp+2]
	add rsp, 10
	shl rax, 1
	lea rdi, [rdi+rax*8] ; now rdi contains a pointer to our IDT Gate

	mov rax, rcx
	and rax, 0xFFFF
	shl ebx, 16
	or rax, rbx
	and rdx, 0xFFFF ; supposed to work... needs extensive testing
	shl rdx, 32
	or rax, rdx
	shr rcx, 16
	mov rbx, rcx
	shl rbx, 48
	or rax, rbx
	stosq
	shr rcx, 16
	xor rax, rax
	or rax, rcx
	stosq
	xor rax, rax
ret

.index_error:
	add rsp, 10
	mov rax, -1
ret

; ax - Gate №

; return values:
; ax - Gate №
; bx - selector
; dl - ist
; dh - type attributes
; rcx - ISR address

; rax - -1 on error

read_IDT_gate:
	cld
	sub rsp, 10
	sidt [rsp]

	shr word [rsp], 4
	cmp ax, word [rsp]
	ja .index_error

	mov rdi, qword [rsp+2]
	add rsp, 10
	shl rax, 1
	lea rdi, [rdi+rax*8]
	shr rax, 1
	mov rbx, qword [rdi]
	shr rbx, 16
	and rbx, 0xFFFF
	;mov rdx, qword [rdi]
	;shr rdx, 32
	mov dl, byte [rdi+4]
	mov dh, byte [rdi+5]
	and rdx, 0xFFFF
	mov rcx, qword [rdi]
	and rcx, 0xFFFF
	mov rsi, qword [rdi]
	shr rsi, 48
	shl rsi, 16
	or rcx, rsi
	mov rsi, qword [rdi+8]
	shl rsi, 32
	or rcx, rsi
ret

.index_error:
	add rsp, 10
	mov rax, -1
ret