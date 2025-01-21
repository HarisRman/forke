global _start
_start:
    mov rax, 1
    push rax
    mov rax, 20
    push rax
    mov rax, 1
    push rax
    push Qword [rsp + 16]
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    test rax, rax
    jz label2
    mov rax, 69
    push rax
    pop rax
    mov [rsp + 8], rax
    add rsp, 0
    jmp label1
label2:
    push Qword [rsp + 8]
    pop rax
    test rax, rax
    jz label3
    push Qword [rsp + 8]
    push Qword [rsp + 8]
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    mov [rsp + 8], rax
    add rsp, 0
    jmp label1
label3:
    mov rax, 42
    push rax
    pop rax
    mov [rsp + 8], rax
label1:
    push Qword [rsp + 8]
    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
