#include "session.h"

void session_start(void) {

  session_t* self = session_init();
  init_sighandle();

  pthread_t sig;
  pthread_create(&sig, NULL, &sighandle_loop, (void *)self);


  session_task_loop(self);

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

void session_task_loop(session_t* self) {
  packet_t pkt;
  bool continue_loop = true;
  while(continue_loop) {
    pkt = jobq_recv(self->jobq);
    switch (pkt.type) {
      case MESSAGE:
        handle_message_packet(pkt);
        break;
      case QUIT_SESSION:
        continue_loop = false;
        break;
      case DETACH_SESSION:
        // detach session
        break;
      case CHILD_DIED:
        // pkt.child died
        break;
      default:
        fprintf(stderr, "Unknown format pkt in jobqueue\n");
        continue_loop = false;
        break;
    }
  }
}

bool handle_message_packet(packet_t pkt) {
  ssize_t writelen = write(pkt.dest,  pkt.payload,  pkt.len);
  if ( writelen != (ssize_t)pkt.len ) {
    fprintf(stderr, "I/O error: %s\n", strerror(errno));
    return false;
  }
  return true;
}

