#ifndef HELPER_H
#define HELPER_H

#include "context.h"
#include <stddef.h>

void context_make(Context *ctx, void *stack_base, size_t stack_size,
                  coro_func_t fn, void *arg);

#endif
