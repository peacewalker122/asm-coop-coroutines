# Coroutine POC (C + x86-64 ASM)

Minimal cooperative coroutine runtime that mixes C and hand-written x86-64 assembly to experiment with stackful coroutines.

## Overview
- **Goal**: demonstrate user-space context switching, manual stack setup, and a tiny scheduler.
- **Status**: proof of concept; intended for exploration and debugging, not production workloads.
- **Platform**: Linux, x86-64 System V ABI, GCC + NASM toolchain.

## Repository Layout
- `src/` – core runtime (`coroutine.c`, `switch.asm`, helpers) and sample driver (`main.c`).
- `include/` – headers that describe the context, queue, and coroutine API surfaces.
- `build/` – populated by `make` with object files and binaries.
- `Makefile` – orchestrates C/ASM compilation and links the demo executable.

## Building
1. Install dependencies: `gcc`, `nasm`, `make`.
2. From the repo root:
   ```sh
   make            # builds ./build/main
   make debug      # builds ./build/main-debug with -Og/-g3 and NASM debug info
   ```

## Running the Demo
```sh
./build/main        # or ./build/main-debug for GDB sessions
```
The driver spawns two coroutines:
1. `start_fn` prints messages with sleeps to illustrate cooperative multitasking.
2. `second_fn` streams the contents of `Makefile` to stdout.

After both coroutines yield back, `main` prints a completion message.

## Runtime Architecture
- `Context` (see `include/context.h`) stores callee-saved registers, stack pointer, entry IP, argument, return target, and a `finished` flag.
- `ctx_switch` (`src/switch.asm`) saves the outgoing context (unless it already finished) and loads the incoming context before jumping to its stored `rip`.
- `context_trampoline` prepares the first invocation of a coroutine by calling `fn(arg)` on its private stack, then hands control to `return_ctx` (currently the main context).
- The coroutine API (`src/coroutine.c`) wraps contexts, a global run queue, and helpers: `co_spawn`, `co_yield`, `co_join`, `co_detach`.

## Using the API
```c
if (co_init() != 0) { /* handle error */ }
co_t *c = co_spawn(fn, arg);
co_join(c);           // Switch into the coroutine until it exits
co_detach(c);         // Currently only frees the struct; stacks leak in this POC
```
- `co_yield()` should be called from inside a coroutine to give up the CPU.
- `co_join()` switches from `main` to the coroutine and assumes the coroutine will return to the saved main context.

## Debugging Tips
- Use the debug build: `make debug` → `./build/main-debug`.
- Break on `context_trampoline` in GDB to inspect stack setup and register handoff:
  ```gdb
  gdb ./build/main-debug
  (gdb) break context_trampoline
  (gdb) run
  ```
- Step past `call r10` to confirm control returns through `ctx_switch` into the saved `return_ctx`.

## Current Limitations (POC)
- Stacks: hard-coded size, allocated with `malloc`, no guard pages, not reclaimed after coroutine exit.
- Scheduler: single FIFO queue, no prioritisation or fairness; `co_run()` is not implemented.
- Resource handling: no cleanup of queued coroutines on shutdown; error cases leak memory.
- Portability: only works on x86-64 System V; assembly must be rewritten for other ABIs.

## Future Improvements
1. Track stack base/size in each coroutine, add guard pages (`mmap` + `mprotect`), and recycle stacks.
2. Flesh out `co_run()` and add richer scheduling policies (round-robin, priorities).
3. Add lifecycle hooks for cleanup, reference counting, or automatic detachment.
4. Write automated tests for edge cases (deep recursion, nested yields, concurrent joins).
5. Explore segmented/dynamic stacks for more robust workloads.

---
This codebase is intentionally minimal to highlight the mechanics of cooperative coroutines; treat it as a learning playground rather than a ready-made runtime.
