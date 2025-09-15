// coroutine.h — tiny cooperative coroutines (no I/O abstractions)

#include <stddef.h>

typedef struct co co_t;
typedef void (*co_fn)(void *arg);

// runtime
int co_init(void); // optional; set up queues, stacks pool
void co_run(void); // drain the run queue until empty

// coroutines
co_t *co_spawn(co_fn fn, void *arg); // create + enqueue a coroutine
void co_yield (void);                // give up the CPU voluntarily

// lifecycle (simple, still no syscalls)
void co_detach(co_t *c); // fire-and-forget
void co_join(co_t *c);   // wait for c to finish (by yielding)
