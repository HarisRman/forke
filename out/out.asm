section .text
	global _start
_start:
    sub rsp, 1
    mov rax, 50
    mov byte [rsp], al
    mov al, byte [rsp]
    and rax, 0xff
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label1:
    cmp rax, 0
    jz label2
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label1
label2:
    cmp rdx, rcx
    jz label3
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label2
label3:
    mov rsi, buf
    mov byte [buf+rdx], 0xA
    add rdx, 1
    mov rax, 1
    mov rdi, 1
    syscall

    mov rax, 60
    mov rdi, 0
    syscall


section .bss
    buf resb 5


section .data
