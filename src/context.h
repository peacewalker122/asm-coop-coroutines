#include <stdint.h>
typedef void (*coro_func_t)(void *);

typedef struct Context {
  uint64_t rbx;  // 0
  uint64_t rbp;  // 8
  uint64_t r12;  // 16
  uint64_t r13;  // 24
  uintmax_t r14; // 32
  uintmax_t r15; // callee-saved registers 40
  uint64_t rsp;  // stack pointer [] 48
  uint64_t rip;  // instruction pointer 56

  coro_func_t fn; // function to call on first run 64
  void *arg;      // function arguements 72
} Context;

// Implemented in assembly (we’ll wire these later)
void ctx_switch(Context *from, Context *to);
void context_trampoline(void);
