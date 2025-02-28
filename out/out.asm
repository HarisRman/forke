section .text
	global _start
_start:
    sub rsp, 1
    mov rax, 120
    mov byte [rsp], al
    mov al, byte [rsp]
    and rax, 0xff
    mov rdi, rax
    mov rax, 60
    syscall

    mov rax, 60
    mov rdi, 0
    syscall


section .bss


section .data
