#include "context.h"
#include <stdio.h>

// Declare the helper from helper.c
void context_make(Context *ctx, void *stack_base, size_t stack_size,
                  coro_func_t fn, void *arg);

// Dummy entry for now
static void start_fn(void *p) { (void)p; }

int main(void) {
  unsigned char stack[8 * 1024]; // 8 KB coroutine stack
  Context ctx;

  context_make(&ctx, stack, sizeof(stack), start_fn, (void *)0xBabeeee);

  printf("ctx.rsp = %p (mod 16 = %zu)\n", (void *)ctx.rsp,
         (size_t)(ctx.rsp % 16));
  printf("ctx.rip = %p (should equal context_trampoline)\n", (void *)ctx.rip);
  printf("ctx.fn  = %p (should equal start_fn)\n", (void *)ctx.fn);
  printf("ctx.arg = %p (should equal 0xDEADBEEF)\n", ctx.arg);
  return 0;
}
