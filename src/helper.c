#include "../include/helper.h"
#include <stddef.h>
#include <stdint.h>

static inline uintptr_t align_down_16(uintptr_t x) {
  return x & ~(uintptr_t)0xF; // clear low 4 bits → multiple of 16
}

// Prepare a new context to start by jumping into context_trampoline,
// which will call fn(arg) on this stack.
void context_make(Context *ctx, void *stack_base, size_t stack_size,
                  coro_func_t fn, void *arg) {
  // 1) Clear the struct (optional but nice)
  *ctx = (Context){0};

  // 2) Compute top-of-stack (stack grows down)
  uintptr_t top = (uintptr_t)stack_base + stack_size;

  // 3) Align down to 16 bytes (SysV ABI requires 16B alignment before a call)
  top = align_down_16(top);

  ctx->rsp = top;                           // where pushes/calls will write
  ctx->rip = (uintptr_t)context_trampoline; // first jump goes to trampoline
  ctx->fn = fn;                             // entry function for first run
  ctx->arg = arg;                           // argument for first run
  ctx->return_ctx = NULL;                   // no return context initially
}
