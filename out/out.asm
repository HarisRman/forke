section .text
	global _start
_start:
    sub rsp, 4
    sub rsp, 4
    mov rax, rsp
    add rax, 4
    push rax
    mov rax, 10
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    push rax
    mov rax, 59
    push rax
    mov eax, dword [rsp+20]
    pop rbx
    add rax, rbx
    pop rbx
    mov dword [rbx], eax
    mov rax, 10
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    mul rbx
    push rax
    mov eax, dword [rsp+12]
    pop rbx
    add rax, rbx
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
    mov eax, dword [rsp+4]
    mov rdi, rax
    mov rax, 60
    syscall

    mov rax, 60
    mov rdi, 0
    syscall


section .bss
    buf resb 5


section .data
