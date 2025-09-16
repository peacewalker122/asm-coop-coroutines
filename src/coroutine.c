#include "../include/coroutine.h"
#include "../include/context.h"
#include "../include/helper.h"
#include "../include/queue.h"
#include <stdbool.h>
#include <stdlib.h>

struct co {
  Context ctx;   // execution context (registers, stack pointer, etc.)
  Queue *runq;   // run queue this coroutine is enqueued on
  co_t *next;    // next coroutine in the run queue
  bool finished; // has this coroutine finished execution?
} cot_t;

static Queue *global_runq = NULL;
static bool is_initialized = false;
static co_t *current_co = NULL;
static Context main_ctx = {0};
static unsigned char main_stack[64 * 1024];
static unsigned char coro_stack[4 * 1024];

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

co_t *co_spawn(co_fn fn, void *arg) {
  if (!is_initialized)
    return NULL; // not initialized

  co_t *coro = (co_t *)malloc(sizeof(co_t));
  if (!coro)
    return NULL; // allocation failed

  coro->runq = global_runq;
  context_make(&coro->ctx, coro_stack, sizeof(coro_stack), fn, arg);
  coro->ctx.return_ctx = &main_ctx; // return to main context when done

  if (queue_enqueue(global_runq, coro) != 0) {
    free(coro);
    return NULL; // enqueue failed
  }

  return coro;
}

void co_yield(void) {
  if (!current_co)
    return; // no current coroutine to yield from

  // Save current context and switch to main context
  yield(&current_co->ctx, &main_ctx);
}

void co_detach(co_t *c) {
  if (!c)
    return; // invalid coroutine

  // Simply free the coroutine structure
  free(c);
}

void co_join(co_t *c) {
  if (!c || c->finished)
    return; // invalid coroutine or already finished

  // Save current context and switch to the coroutine's context
  current_co = c;
  yield(&main_ctx, &c->ctx);

  // When we return here, the coroutine has finished
  c->finished = true;
  current_co = NULL;
}
