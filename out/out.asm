global _start
_start:
    mov rax, 19
    push rax
label1:
    mov rax, 10
    push rax
    push Qword [rsp + 8]
    pop rax
    pop rbx
    cmp rax, rbx
    je label3
    push 1
    jmp label4
label3:
    push 0
label4:
    pop rax
    test rax, rax
    jz label2
    mov rax, 1
    push rax
    push Qword [rsp + 8]
    pop rax
    pop rbx
    sub rax, rbx
    push rax
    pop rax
    mov [rsp + 0], rax
    add rsp, 0
    jmp label1
label2:
    push Qword [rsp + 0]
    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
