#include "../include/coroutine.h"
#include <stdio.h>
#include <threads.h>

// Dummy entry for now
void start_fn(void *p) {
  // print the value passed to the coroutine
  printf("[coro] started with arg: %s\n", (char *)p);

  // sleep for few seconds
  for (int i = 0; i < 3; i++) {
    printf("[coro] running with arg: %s\n", (char *)p);
    // simulate doing some work
    thrd_sleep(&(struct timespec){.tv_sec = 1}, NULL);
  }

  puts("[coro] to main");
}

void second_fn(void *p) {
  // reading file and print its content
  char *arg = (char *)p;
  printf("[coro2] trying to access file: %s\n", arg);

  FILE *file = fopen(arg, "r");
  if (file == NULL) {
    perror("Failed to open file");
    return;
  }

  char buffer[256];
  while (fgets(buffer, sizeof(buffer), file)) {
    printf("[coro2] read line: %s", buffer);
  }
  fclose(file);

  printf("[coro2] finished reading file: %s\n", (char *)p);
  puts("[coro2] to main");
}

int main(void) {
  if (co_init() != 0) {
    fprintf(stderr, "Failed to initialize coroutine system\n");
    return 1;
  }

  co_t *coroutine1 =
      co_spawn(start_fn, "Jokowi, baik hati dan bijaksana, senang membantu "
                         "rakyatnya. Ucap seorang bapak-bapak dari fakfak.");
  if (!coroutine1) {
    fprintf(stderr, "Failed to spawn coroutine\n");
    return 1;
  }
  co_t *coroutine2 = co_spawn(second_fn, "Makefile");
  if (!coroutine2) {
    fprintf(stderr, "Failed to spawn coroutine\n");
    return 1;
  }

  co_join(coroutine1);
  co_join(coroutine2);

  puts("[main] back from coroutine");
  puts("[main] done");

  return 0;
}
