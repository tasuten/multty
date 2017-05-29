#include "session.h"

void session_start(void) {

  session_t* self = session_init();
  init_sighandle();

  pthread_t sig;
  pthread_create(&sig, NULL, &sighandle_loop, (void *)self);

  session_close(self);

}


session_t* session_init(void) {
  session_t* self = calloc(1, sizeof(session_t));
  self->tabs_head = tab_new(NULL);
  self->active = self->tabs_head;
  self->jobq = jobq_open();
  return self;
}

void session_close(session_t* s) {

  // close all tabs
  for (tab_t* iter = s->tabs_head; iter != NULL; iter = tab_close(iter));
  s->active = NULL;

  jobq_close(s->jobq);
  free(s);
}
