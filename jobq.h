#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define READ 0
#define WRITE 1
#define PAYLOAD_LEN 256

typedef enum __type {
  MESSAGE,
  FINISH,
} packet_type;

typedef struct __packet {
  packet_type type;
  char payload[PAYLOAD_LEN];
  size_t len; // payload data length
  int dest;
} packet_t;

int* jobq_open(void);
void jobq_close(int* queue);
packet_t jobq_recv(int* queue);
void jobq_send(int* queue, const packet_t pkt);
