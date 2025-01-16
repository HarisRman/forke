global _start
_start:

    mov rax, 50
    push rax

    mov rax, 20
    push rax

    push Qword [rsp + 0]
    mov rax, 60
    pop rdi
    syscall
    add rsp, 8
    push Qword [rsp + 0]
    mov rax, 60
    pop rdi
    syscall

    mov rax, 60
    mov rdi, 0
    syscall
