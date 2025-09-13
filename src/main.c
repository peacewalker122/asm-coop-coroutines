#include "context.h"
#include <stdio.h>

// Declare the helper from helper.c
void context_make(Context *ctx, void *stack_base, size_t stack_size,
                  coro_func_t fn, void *arg);

static Context main_ctx;
static Context coro_ctx;
static unsigned char coro_stack[8 * 1024];
static void yield(Context *from, Context *to) { ctx_switch(from, to); }

// Dummy entry for now
static void start_fn(void *p) {
  // print the value passed to the coroutine

  printf("[coro] started with arg: %s\n", (char *)p);

  // Yield back to main context - this should cause return to main
  yield(&coro_ctx, &main_ctx);

  printf("[coro] resumed after first yield\n");

  // Yield again
  yield(&coro_ctx, &main_ctx);

  printf("[coro] resumed after second yield\n");

  yield(&coro_ctx, &main_ctx);
}

// NOTE:
// Current approach weren't made the coro-ctx being child of main-ctx, so the
// program could quit using coro-ctx without returning to main-ctx

int main(void) {
  main_ctx = (Context){0}; // Initialize main context
  context_make(&coro_ctx, coro_stack, sizeof(coro_stack), start_fn,
               (void *)"This give me a worst nightmare");

  // check the rsp is it aligned to 16 bytes
  if (coro_ctx.rsp % 16 != 0) {
    puts("Stack pointer is not aligned to 16 bytes!");
    return 1;
  }

  // check the function pointer and argument are set
  printf("Function pointer: %p\n", (void *)coro_ctx.fn);
  printf("Argument: %p\n", coro_ctx.arg);

  puts("[main] to coro");
  yield(&main_ctx, &coro_ctx);
  puts("[main] back 1");
  yield(&main_ctx, &coro_ctx);
  puts("[main] back 2");
  yield(&main_ctx, &coro_ctx);

  puts("[main] done");
}
