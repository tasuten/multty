#include "session.h"

void session_start(void) {
  session_t* self = calloc(1, sizeof(session_t));

  self->tabs_head = tab_new(NULL);
  self->active = self->tabs_head;
  self->jobq = jobq_open();



  signal_handle_thread(self);
  pthread_t _in;
  pthread_create(&_in, NULL, &stdin_handler, (void *)self);
  pthread_t _tty;
  pthread_create(&_tty, NULL, &tty_handler, (void *)self);


  pthread_t consumer;
  pthread_create(&consumer, NULL, &consume_queue, (void *)self);
  pthread_join(consumer, NULL);

  jobq_close(self->jobq);
  free(self); self = NULL;
}

void* stdin_handler(void* s) {
  pthread_detach(pthread_self());
  session_t* self = (session_t*)s;
  ssize_t nread = 0;
  char buf[PAYLOAD_MAX];
  packet_t pkt;
  while(1) {
    nread = read(STDIN_FILENO,  buf,  PAYLOAD_MAX);

    if (nread < 0 || nread == 0) break;

    strncpy(pkt.payload, buf, nread);
    pkt.type = MESSAGE;
    pkt.dest = self->active->tty.fd;
    pkt.len = (size_t)nread;

    jobq_send(self->jobq, pkt);
  }

  return NULL;
}

void* tty_handler(void* s) {
  pthread_detach(pthread_self());
  session_t* self = (session_t*)s;
  ssize_t nread = 0;
  char buf[PAYLOAD_MAX];
  packet_t pkt;
  while(1) {
    nread = read(self->active->tty.fd, buf, PAYLOAD_MAX);

    if (nread < 0 || nread == 0) break;

    strncpy(pkt.payload, buf, nread);
    pkt.type = MESSAGE;
    pkt.dest = STDOUT_FILENO;
    pkt.len = (size_t)nread;
    jobq_send(self->jobq, pkt);

  }

  return NULL;
}

void* consume_queue(void *s) {
  session_t* self = (session_t*)s;
  packet_t pkt;
  while(1) {
    pkt = jobq_recv(self->jobq);

    if (pkt.type == QUIT_SESSION) {
      break;
    }

    if (pkt.type == MESSAGE) {
      write(pkt.dest,  pkt.payload, pkt.len);
    }
  }

  return NULL;
}

