#include <stdint.h>
typedef void (*coro_func_t)(void *);

typedef struct Context {
  uint64_t rbx; // 0
  uint64_t rbp; // 8
  uint64_t r12; // 16
  uint64_t r13; // 24
  uint64_t r14; // 32
  uint64_t r15; // callee-saved registers 40

  uint64_t rsp; // stack pointer [] 48
  uint64_t rip; // address of the return value of the function 56

  coro_func_t fn;             // function to call on first run 64
  void *arg;                  // function arguements 72
  struct Context *return_ctx; // context to return to when coroutine finishes 80
} Context;

// Implemented in assembly (we’ll wire these later)
void ctx_switch(Context *from, Context *to);
void context_trampoline(void);
void coroutine_exit(void);
