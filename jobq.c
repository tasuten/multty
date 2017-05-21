#include "jobq.h"

int* jobq_open(void) {
  int* q = calloc(2, sizeof(int));
  if (pipe(q) < 0) {
    fprintf(stderr, "Opening pipe failed: %s", strerror(errno));
    return NULL;
  }

  return q;
}

void jobq_close(int* queue) {
  close(queue[READ]);
  close(queue[WRITE]);
  free(queue);
}

packet_t jobq_recv(int* queue) {
  packet_t pkt;
  ssize_t readlen = read(queue[READ], &pkt, sizeof(pkt));
  if (readlen == 0) {
    pkt.type = FIN;
  } else if (readlen != sizeof(pkt)) {
    fprintf(stderr, "reading jobqueue failed: %s", strerror(errno));
    jobq_close(queue);
    exit(EXIT_FAILURE);
  }
  return pkt;
}

void jobq_send(int* queue, packet_t pkt) {
  ssize_t writelen = write(queue[WRITE], &pkt, sizeof(pkt));
  if (writelen != sizeof(pkt)) {
    fprintf(stderr, "writing jobqueue failed: %s", strerror(errno));
    jobq_close(queue);
    exit(EXIT_FAILURE);
  }
}
