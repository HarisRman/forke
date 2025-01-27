global _start
_start:
    mov rax, 50
    push rax
    mov rax, 20
    push rax
    push Qword [rsp + 0]
    push Qword [rsp + 16]
    pop rax
    pop rbx
    cmp rax, rbx
    jnc label3
    push 1
    jmp label4
label3:
    push 0
label4:
    pop rax
    test rax, rax
    jz label2
    mov rax, 69
    push rax
    mov rax, 60
    pop rdi
    syscall
    add rsp, 8
    jmp label1
label2:
label1:
    mov rax, 70
    push rax
    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
