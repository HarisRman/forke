section .text
	global _start
_start:
    sub rsp, 40
    sub rsp, 4
    mov rax, rsp
    push rax
    mov rax, 0
    pop rbx
    mov dword [rbx], eax
label1:
    mov rax, 10
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    cmp rax, rbx
    jnc label3
    mov rax, 1
    jmp label4
label3:
    mov rax, 0
label4:
    test rax, rax
    jz label2
    mov rax, rsp
    add rax, 4
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    mov rcx, 4
    mul rcx
    add rbx, rax
    mov rax, rbx
    push rax
    mov eax, dword [rsp+8]
    push rax
    mov eax, dword [rsp+16]
    pop rbx
    mul rbx
    pop rbx
    mov dword [rbx], eax
    mov rax, rsp
    push rax
    mov rax, 1
    push rax
    mov eax, dword [rsp+16]
    pop rbx
    add rax, rbx
    pop rbx
    mov dword [rbx], eax
    add rsp, 0
    jmp label1
label2:
    mov rax, rsp
    push rax
    mov rax, 0
    pop rbx
    mov dword [rbx], eax
    mov rsi, label5
    mov rdx, 13
    mov rax, 1
    mov rdi, 1
    syscall
label6:
    mov rax, 10
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    cmp rax, rbx
    jnc label8
    mov rax, 1
    jmp label9
label8:
    mov rax, 0
label9:
    test rax, rax
    jz label7
    mov rax, rsp
    add rax, 4
    push rax
    mov eax, dword [rsp+8]
    pop rbx
    mov rcx, 4
    mul rcx
    add rbx, rax
    mov rax, rbx
    mov eax, dword [rax]
    mov rdi, 10
    mov rcx, 0
    mov rdx, 0
label10:
    cmp rax, 0
    jz label11
    div rdi
    add rdx, 0x30
    push rdx
    mov rdx, 0
    add rcx, 1
    jmp label10
label11:
    cmp rdx, rcx
    jz label12
    pop rax
    mov byte [buf+rdx], al
    add rdx, 1
    jmp label11
label12:
    mov rsi, buf
    mov byte [buf+rdx], 0xA
    add rdx, 1
    mov rax, 1
    mov rdi, 1
    syscall
    mov rax, rsp
    push rax
    mov rax, 1
    push rax
    mov eax, dword [rsp+16]
    pop rbx
    add rax, rbx
    pop rbx
    mov dword [rbx], eax
    add rsp, 0
    jmp label6
label7:
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
	label5 db 'Your Array: ' , 0xA
