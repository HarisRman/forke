section .text
	global _start
_start:
    mov rax, 5
    push rax
    mov rax, 1
    push rax
label1:
    mov rax, 11
    push rax
    push Qword [rsp + 8]
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
    push Qword [rsp + 8]
    pop rax
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label5:
    cmp rax, 0
    jz label6
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label5
label6:
    cmp rdx, rcx
    jz label7
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label6
label7:
    mov rsi, buf
    mov rax, 1
    mov rdi, 1
    syscall
    mov rsi, label8
    mov rdx, 3
    mov rax, 1
    mov rdi, 1
    syscall
    push Qword [rsp + 0]
    pop rax
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label9:
    cmp rax, 0
    jz label10
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label9
label10:
    cmp rdx, rcx
    jz label11
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label10
label11:
    mov rsi, buf
    mov rax, 1
    mov rdi, 1
    syscall
    mov rsi, label12
    mov rdx, 3
    mov rax, 1
    mov rdi, 1
    syscall
    push Qword [rsp + 0]
    push Qword [rsp + 16]
    pop rax
    pop rbx
    mul rbx
    push rax
    pop rax
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label13:
    cmp rax, 0
    jz label14
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label13
label14:
    cmp rdx, rcx
    jz label15
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label14
label15:
    mov rsi, buf
    mov byte [buf+rdx], 0xA
    add rdx, 1
    mov rax, 1
    mov rdi, 1
    syscall
    mov rax, 1
    push rax
    push Qword [rsp + 8]
    pop rax
    pop rbx
    add rax, rbx
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


section .bss
    buf resb 5


section .data
	label8 db ' x '
	label12 db ' = '
