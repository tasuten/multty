#include "session.h"

void session_start(void) {
  session_t* self = calloc(1, sizeof(session_t));
  self->tabs_head = tab_new(NULL);
  self->active = self->tabs_head;
  self->active_mutex = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(self->active_mutex , NULL);

  init_sighandle();

  pthread_t sig;
  pthread_create(&sig, NULL, &sighandle_loop, (void *)self);

  while(1);

}

