#include "context.h"
#include <stdio.h>
#include <threads.h>

// Declare the helper from helper.c
void context_make(Context *ctx, void *stack_base, size_t stack_size,
                  coro_func_t fn, void *arg);

static Context main_ctx;
static Context coro_ctx;
static Context coro2_ctx;
static unsigned char coro_stack[8 * 1024];
static void yield(Context *from, Context *to) { ctx_switch(from, to); }

// Dummy entry for now
static void start_fn(void *p) {
  // print the value passed to the coroutine
  printf("[coro] started with arg: %s\n", (char *)p);

  // sleep for few seconds
  for (int i = 0; i < 3; i++) {
    puts("[coro] running");
    // simulate doing some work
    thrd_sleep(&(struct timespec){.tv_sec = 1}, NULL);
  }

  puts("[coro] to main");

  // TODO: the goals is to return to main_ctx without have to expliclity yield
  // it

  yield(&coro_ctx, &main_ctx);
}

// NOTE:
// Current approach weren't made the coro-ctx being child of main-ctx, so the
// program could quit using coro-ctx without returning to main-ctx
//
// so yield here act as a program to switch between context, from a ctx to
// another ctx.

int main(void) {
  main_ctx = (Context){0}; // Initialize main context
  context_make(&coro_ctx, coro_stack, sizeof(coro_stack), start_fn,
               (void *)"This give me a worst nightmare");
  context_make(&coro2_ctx, coro_stack, sizeof(coro_stack), start_fn,
               (void *)"This give me a worst nightmare 2");

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
  puts("[main] back from coro");

  puts("[main] to coro2");
  yield(&main_ctx, &coro2_ctx);
  puts("[main] back from coro2");

  puts("[main] done");
}
