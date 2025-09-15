#include "../include/context.h"
#include "../include/helper.h"
#include <stdio.h>
#include <threads.h>

static Context main_ctx;
static Context coro_ctx;
static unsigned char coro_stack[8 * 1024];
static void yield(Context *from, Context *to) { ctx_switch(from, to); }

// NOTE: For creating new function thats abstract the context switch, there's
// some little things that could be noted.
// 1. The function should generate a new stack frame, so it should be marked as
// noinline
// 2. The function should be marked as noreturn, so the compiler won't expect it

// Dummy entry for now
void start_fn(void *p) {
  // made new context_make
  Context fn_coro_ctx = {0};
  context_make(&fn_coro_ctx, coro_stack, sizeof(coro_stack), start_fn,
               (void *)"This give me a worst nightmare 2");

  // print the value passed to the coroutine
  printf("[coro] started with arg: %s\n", (char *)p);

  // sleep for few seconds
  for (int i = 0; i < 3; i++) {
    printf("[coro] running with arg: %s\n", (char *)p);
    // simulate doing some work
    thrd_sleep(&(struct timespec){.tv_sec = 1}, NULL);
  }

  puts("[coro] to main");

  // TODO: the goals is to return to main_ctx without have to expliclity yield
  // it

  yield(&fn_coro_ctx, &main_ctx);
}

/*
 * NOTE:
 Current approach weren't made the coro-ctx being child of main-ctx, so the
 program could quit using coro-ctx without returning to main-ctx
 * 1 Create new event loop to manage coroutine lifecycle
 * 2. Entrypoint for coroutine were an queue of function to be executed
 * 3. Coroutine could yield to main-ctx, and main-ctx could yield to
 */

//
// so yield here act as a program to switch between context, from a ctx to
// another ctx.

int main(void) {
  main_ctx = (Context){0}; // Initialize main context

  // Create coroutine context
  Context coro_ctx = {0};
  context_make(&coro_ctx, coro_stack, sizeof(coro_stack), start_fn,
               (void *)"Hello from coroutine!");

  puts("[main] starting coroutine");
  yield(&main_ctx, &coro_ctx);

  puts("[main] back from coroutine");
  puts("[main] done");

  return 0;
}
