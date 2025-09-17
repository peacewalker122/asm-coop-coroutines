; switch.asm — minimal placeholders so we can link the test
global ctx_switch
global context_trampoline
section .text

; rdi = from, rsi = to  (to unused in save-only)
ctx_switch:
    ; SAVE current CPU state into *from
    mov     [rdi+0],  rbx
    mov     [rdi+8],  rbp
    mov     [rdi+16], r12
    mov     [rdi+24], r13
    mov     [rdi+32], r14
    mov     [rdi+40], r15

    mov     [rdi+48], rsp          ; save current stack pointer
    mov     rax, [rsp]             ; caller's return address = saved RIP
    mov     [rdi+56], rax          ; save return address

    mov   rbx, [rsi+0]             ; load new CPU state from *to
    mov   rbp, [rsi+8]
    mov   r12, [rsi+16]
    mov   r13, [rsi+24]
    mov   r14, [rsi+32]
    mov   r15, [rsi+40]
    mov   rsp, [rsi+48]            ; restore new stack pointer
    mov   rax, [rsi+56]            ; restore return address
    
    mov   rdi, rsi       ; set rdi to point to the target context for trampoline
    jmp rax
.ctx_exit:
    ret

; this function below expect the rdi register to point to a context structure
context_trampoline:
    mov r13, rdi

    mov r10, [r13 + 64] ; load the function pointer from the context (fn)
    mov r11, [r13 + 72] ; load the function arguments from the context (arg)
    mov r12, [r13 + 80] ; load return_ctx from the context
    
    mov rdi, r11        ; move the first argument into rdi
    call r10 ; call the function
    mov byte [r13 + 88], 1

    test r12, r12
    je .exit ; if return_ctx is NULL, exit

    mov rsi, r12        ; set rsi to return_ctx
    call ctx_switch     ; switch back to return context
.exit:
    ; Exit cleanly if no return context
    mov rax, 60         ; sys_exit system call
    mov rdi, 1          ; exit status 0
    syscall
