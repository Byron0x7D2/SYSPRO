#include <pthread.h>

struct args {
  int x;
  double y;
};

void worker(void *_args) {
  struct args* args = _args;
  fun(args->x, args->y);
}

int main(void) {
  struct args args = {.x = 2, .y = 3.0};
  pthread_t tid;
  pthread_create(&tid, NULL, worker, &args);
  pthread_join(tid, 0);
  return 0;
}