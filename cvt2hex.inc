[BITS 64]
[DEFAULT REL]

; destination format: utf16: '0x', n_digits*2, '\0'
;                                             ^rdi
; rdi - string destination ending, rax - number
align 8
cvt2hex:
	std
	mov rbx, rax
	lea rcx, [.jtbl]
	._loop:
		mov rax, rbx
		and rax, 0xF
		mov ax, word [rcx+rax*2]
		stosw
		shr rbx, 4
	jnz ._loop
	cmp word [rdi], __utf16__(`x`)
	jz ._end
	mov ax, __utf16__(`0`)
	._loop2:
		stosw
		cmp word [rdi], __utf16__(`x`)
	jnz ._loop2
	._end:
	cld
	dec rdi
ret
.jtbl dw __utf16__(`0123456789ABCDEF`)

