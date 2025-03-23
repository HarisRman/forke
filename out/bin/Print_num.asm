section .text
	global _start
_start:
    sub rsp, 10
    sub rsp, 4
    sub rsp, 4
    mov rax, rsp
    add rax, 4
    push rax
    mov rax, 69428
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    push rax
    mov rax, 0
    pop rbx
    mov dword [rbx], eax
label1:
    mov rax, 0
    push rax
    mov eax, dword [rsp+12]
    pop rbx
    cmp rax, rbx
    je label3
    mov rax, 1
    jmp label4
label3:
    mov rax, 0
label4:
    test rax, rax
    jz label2
    mov rax, rsp
    add rax, 8
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    mov rcx, 1
    mul rcx
    add rbx, rax
    mov rax, rbx
    push rax
    mov rax, 48
    push rax
    mov rax, 10
    push rax
    mov eax, dword [rsp+28]
    pop rbx
    mov rdx, 0
    div rbx
    mov rax, rdx
    pop rbx
    add rax, rbx
    pop rbx
    mov byte [rbx], al
    mov rax, rsp
    add rax, 4
    push rax
    mov rax, 10
    push rax
    mov eax, dword [rsp+20]
    pop rbx
    mov rdx, 0
    div rbx
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    mov rbx, rax
    mov rcx, 1
    add dword [rbx], ecx
    add rsp, 0
    jmp label1
label2:
    sub rsp, 10
    sub rsp, 4
    sub rsp, 4
    mov rax, rsp
    add rax, 4
    push rax
    mov rax, 0
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    add rax, 18
    push rax
    mov rax, 1
    push rax
    mov eax, dword [rsp+34]
    pop rbx
    sub rax, rbx
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    push rax
    mov rax, 1
    push rax
    mov rax, 0
    pop rbx
    sub rax, rbx
    pop rbx
    mov dword [rbx], eax
label5:
    mov eax, dword [rsp]
    push rax
    mov eax, dword [rsp+26]
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
    mov rax, rsp
    add rax, 8
    push rax
    mov eax, dword [rsp+12]
    pop rbx
    mov rcx, 1
    mul rcx
    add rbx, rax
    mov rax, rbx
    push rax
    mov rax, rsp
    add rax, 34
    push rax
    mov eax, dword [rsp+34]
    pop rbx
    mov rcx, 1
    mul rcx
    add rbx, rax
    mov rax, rbx
    mov al, byte [rax]
    and rax, 0xff
    pop rbx
    mov byte [rbx], al
    mov rax, rsp
    add rax, 18
    push rax
    mov rax, 1
    push rax
    mov eax, dword [rsp+34]
    pop rbx
    sub rax, rbx
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    add rax, 4
    mov rbx, rax
    mov rcx, 1
    add dword [rbx], ecx
    add rsp, 0
    jmp label5
label6:
    mov rax, rsp
    add rax, 8
    mov rsi, rax
    mov eax, dword [rsp+4]
    mov rdx, rax
    mov rax, 1
    mov rdi, 1
    syscall
    mov rsi, label9
    mov rdx, 1
    mov rax, 1
    mov rdi, 1
    syscall
    mov rax, 69
    mov rdi, rax
    mov rax, 60
    syscall

    mov rax, 60
    mov rdi, 0
    syscall


section .bss


section .data
	label9 db '' , 0xA
