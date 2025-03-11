section .text
	global _start
_start:
    sub rsp, 4
    sub rsp, 4
    sub rsp, 4
    mov rax, rsp
    add rax, 8
    push rax
    mov rax, 2
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    add rax, 4
    push rax
    mov rax, 11
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    push rax
    mov rax, 0
    pop rbx
    mov dword [rbx], eax
    mov rsi, label1
    mov rdx, 9
    mov rax, 1
    mov rdi, 1
    syscall
    mov eax, dword [rsp+8]
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label2:
    cmp rax, 0
    jz label3
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label2
label3:
    cmp rdx, rcx
    jz label4
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label3
label4:
    mov rsi, buf
    mov byte [buf+rdx], 0xA
    add rdx, 1
    mov rax, 1
    mov rdi, 1
    syscall
label5:
    mov eax, dword [rsp+4]
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    cmp rax, rbx
    jnc label7
    mov rax, 1
    jmp label8
label7:
    mov rax, 0
label8:
    test rax, rax
    jz label6
    mov eax, dword [rsp+8]
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
    mov eax, dword [rsp]
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
    mov rax, 1
    mov rdi, 1
    syscall
    mov rsi, label16
    mov rdx, 3
    mov rax, 1
    mov rdi, 1
    syscall
    mov eax, dword [rsp]
    push rax
    mov eax, dword [rsp+16]
    pop rbx
    mul rbx
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label17:
    cmp rax, 0
    jz label18
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label17
label18:
    cmp rdx, rcx
    jz label19
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label18
label19:
    mov rsi, buf
    mov byte [buf+rdx], 0xA
    add rdx, 1
    mov rax, 1
    mov rdi, 1
    syscall
    mov rax, rsp
    mov rbx, rax
    add dword [rbx], 1
    add rsp, 0
    jmp label5
label6:
    mov rax, 69
    mov rdi, rax
    mov rax, 60
    syscall

    mov rax, 60
    mov rdi, 0
    syscall


section .bss
    buf resb 5


section .data
	label1 db 'Table of '
	label12 db ' x '
	label16 db ' = '
