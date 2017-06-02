#include "jobq.h"

mpsc_t* jobq_open(void) {
  mpsc_t* q = calloc(1, sizeof(mpsc_t));
  q->pipe = calloc(2, sizeof(int));
  if (pipe(q->pipe) < 0) {
    fprintf(stderr, "Opening pipe failed: %s", strerror(errno));
    return NULL;
  }

  q->write_lock = calloc(1, sizeof(pthread_mutex_t));
  pthread_mutex_init(q->write_lock, NULL);
  return q;
}

void jobq_close(mpsc_t* queue) {
  close(queue->pipe[READ]);
  close(queue->pipe[WRITE]);
  pthread_mutex_destroy(queue->write_lock);
  free(queue->pipe);
  free(queue->write_lock);
  free(queue);
}

packet_t jobq_recv(mpsc_t* queue) {
  packet_t pkt;
  // sigle consumer, so no need mutal exclusion
  ssize_t readlen = read(queue->pipe[READ], &pkt, sizeof(pkt));
  if (readlen == 0) { // no writer exists
    pkt.type = QUIT_SESSION;
  } else if (readlen != sizeof(pkt)) {
    fprintf(stderr, "reading jobqueue failed: %s", strerror(errno));
    jobq_close(queue);
    exit(EXIT_FAILURE);
  }
  return pkt;
}

void jobq_send(mpsc_t* queue, const packet_t pkt) {
  pthread_mutex_lock(queue->write_lock);
  ssize_t writelen = write(queue->pipe[WRITE], &pkt, sizeof(pkt));
  pthread_mutex_unlock(queue->write_lock);
  if (writelen != sizeof(pkt)) {
    fprintf(stderr, "writing jobqueue failed: %s", strerror(errno));
    jobq_close(queue);
    exit(EXIT_FAILURE);
  }
}
