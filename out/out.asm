global _start
_start:
    mov rax, 1
    push rax
    mov rax, 1
    push rax
    push Qword [rsp + 8]
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    test rax, rax
    jz label2
    mov rax, 99
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp label1
label2:
    push Qword [rsp + 0]
    pop rax
    test rax, rax
    jz label3
    mov rax, 69
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
    jmp label1
label3:
    mov rax, 10
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 0
label1:
    mov rax, 20
    push rax
    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
