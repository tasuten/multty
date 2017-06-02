#include "session.h"

void session_start(void) {

  session_t* self = session_init();
  init_sighandle();

  pthread_t sig;
  pthread_create(&sig, NULL, &sighandle_loop, (void *)self);
  pthread_t in;
  pthread_create(&in, NULL, &stdin_loop, (void *)self);
  pthread_t out;
  pthread_create(&out, NULL, &ttyout_loop, (void *)self);

  session_task_loop(self);

  session_close(self);

}


session_t* session_init(void) {
  session_t* self = calloc(1, sizeof(session_t));
  self->tabs_head = tab_new(NULL);
  self->active = self->tabs_head;
  self->jobq = jobq_open();
  self->rwlock = calloc(1, sizeof(pthread_rwlock_t));
  pthread_rwlock_init(self->rwlock, NULL);
  return self;
}

void session_close(session_t* s) {

  // close all tabs
  for (tab_t* iter = s->tabs_head; iter != NULL; iter = tab_close(iter));
  s->active = NULL;

  pthread_rwlock_destroy(s->rwlock);
  free(s->rwlock);

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
        pthread_rwlock_rdlock(self->rwlock);
        continue_loop = handle_message_packet(pkt);
        pthread_rwlock_unlock(self->rwlock);
        break;
      case QUIT_SESSION:
        continue_loop = false;
        break;
      case DETACH_SESSION:
        // detach session
        break;
      case CHILD_DIED:
        pthread_rwlock_wrlock(self->rwlock);
        continue_loop = handle_child_died_packet(pkt, self);
        pthread_rwlock_unlock(self->rwlock);
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


bool handle_child_died_packet(packet_t pkt, session_t* self) {
  pid_t died = pkt.child;
  bool continue_session = false;

  for (tab_t* iter = self->tabs_head; iter != NULL; iter = iter->next ) {
    if ( iter->tty.pid == died ) {
      self->active = iter->next;
      tab_close(iter);
      if (self->active != NULL) {
        continue_session = true;
      }
      // ToDo: refresh screen
      break;
    }
  }

  return continue_session;
}

void* stdin_loop(void* s) {
  pthread_detach(pthread_self());
  session_t* self = (session_t*)s;

  packet_t pkt;

  while (1) {

    pkt.type = MESSAGE;

    ssize_t readlen = read(STDIN_FILENO, pkt.payload, PAYLOAD_MAX);

    if (readlen <= 0) {
      fprintf(stderr, "STDIN I/O error: %s\n", strerror(errno));
      pkt.type = QUIT_SESSION;
      jobq_send(self->jobq, pkt);
      break;
    }

    pkt.len = (size_t)readlen;
    pthread_rwlock_rdlock(self->rwlock);
    pkt.dest = self->active->tty.fd;
    pthread_rwlock_unlock(self->rwlock);

    jobq_send(self->jobq, pkt);

  }

  return NULL;
}

void* ttyout_loop(void* s) {
  pthread_detach(pthread_self());
  session_t* self = (session_t*)s;

  struct pollfd fds[1];


  while (1) {
    pthread_rwlock_rdlock(self->rwlock);
    fds[0].fd = self->active->tty.fd;
    pthread_rwlock_unlock(self->rwlock);
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    if (poll(fds, 1, -1) != 1) break;

    if (! fds[0].revents & POLLIN) break;

    packet_t pkt;
    pkt.type = MESSAGE;

    pkt.dest = STDOUT_FILENO;

    ssize_t readlen = read(fds[0].fd, pkt.payload, PAYLOAD_MAX);

    if (readlen <= 0) {
      pkt.type = QUIT_SESSION;
      jobq_send(self->jobq, pkt);
      break;
    }

    pkt.len = (size_t)readlen;

    jobq_send(self->jobq, pkt);

  }

  return NULL;
}
