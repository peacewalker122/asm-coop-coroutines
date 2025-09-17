#include "../include/coroutine.h"
#include "../include/context.h"
#include "../include/helper.h"
#include "../include/queue.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct co {
  Context ctx; // execution context (registers, stack pointer, etc.)
  Queue *runq; // run queue this coroutine is enqueued on
  co_t *next;  // next coroutine in the run queue
} cot_t;

static Queue *global_runq = NULL;
static bool is_initialized = false;
static co_t *current_co = NULL;
static size_t coro_stack_size = 4 * 1024; // 4KB stack size
static Context main_ctx = {0};            // main execution context

static void yield(Context *from, Context *to) { ctx_switch(from, to); }

int co_init(void) {
  if (is_initialized)
    return 0; // already initialized

  global_runq = queue_init();
  if (!global_runq)
    return -1; // failed to create run queue

  is_initialized = true;
  return 0;
}

void co_run(void) {
  if (!is_initialized)
    return; // not initialized

  while (true) {
    co_t *next_coro = (co_t *)queue_dequeue(global_runq);
    if (!next_coro)
      break; // run queue is empty

    current_co = next_coro;
    printf("executing coroutine %p\n", (void *)next_coro);
    yield(&main_ctx, &next_coro->ctx);

    // if the coroutine has finished, free its resources
    if (next_coro->ctx.finished) {
      free(next_coro);
    }
  }
}

co_t *co_spawn(co_fn fn, void *arg) {
  if (!is_initialized)
    return NULL; // not initialized

  co_t *coro = (co_t *)malloc(sizeof(co_t));
  if (!coro)
    return NULL; // allocation failed

  // allocate stack for the coroutine
  uint8_t *coro_stack = (uint8_t *)malloc(4 * 1024); // 64KB stack
  if (!coro_stack)
    return NULL;

  coro->runq = global_runq;
  context_make(&coro->ctx, coro_stack, coro_stack_size, fn, arg);
  coro->ctx.return_ctx = &main_ctx; // return to main context when done

  if (queue_enqueue(global_runq, coro) != 0) {
    free(coro);
    return NULL; // enqueue failed
  }

  return coro;
}

// resume the current execution context
void co_yield(void) {
  if (!current_co)
    return; // no current coroutine to yield from

  // enqueue the current coroutine
  if (queue_enqueue(global_runq, current_co) != 0)
    return; // failed to re-enqueue

  co_t *coro = current_co;
  current_co = NULL; // clear current coroutine
  yield(&coro->ctx, &main_ctx);
}

void co_detach(co_t *c) {
  if (!c)
    return; // invalid coroutine

  // Simply free the coroutine structure
  free(c);
}

// wait for coroutine c to finish by yielding to it
void co_join(co_t *c) {
  if (!c || c->ctx.finished)
    return; // invalid coroutine or already finished

  current_co = c;
  ctx_switch(&main_ctx, &c->ctx);
  current_co = NULL; // clear current coroutine
}
