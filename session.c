#include "session.h"

void session_start(void) {
  session_t* self = calloc(1, sizeof(session_t));
  self->tabs_head = tab_new(NULL);
  self->active = self->tabs_head;

  init_sighandle();

  pthread_t sig;
  pthread_create(&sig, NULL, &sighandle_loop, (void *)self);

  while(1);

}

