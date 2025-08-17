; switch.asm — minimal placeholders so we can link the test
global ctx_switch
global context_trampoline
section .text

; For now, just return immediately
ctx_switch:
    ; store the from context
    mov rbx, [rdi]  ; rdi points to the from context
    mov rbp, [rdi + 8]  ; rdi points to the from context
    mov r12, [rdi + 16] ; rdi points to the from context
    mov r13, [rdi + 24] ; rdi points to the from context
    mov r14, [rdi + 32] ; rdi points to the from context
    mov r15, [rdi + 40] ; rdi points to the from context
    mov rsp, [rdi + 48] ; rdi points to the from context

    mov rip, [rsp]

    ret

; We won’t actually run this yet; it’s just to satisfy the linker
context_trampoline:
    ret
