#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define PAYLOAD_MAX 256
#define READ 0
#define WRITE 1

typedef enum __type {
  MESSAGE,
  QUIT_SESSION,
  DETACH_SESSION,
  CHILD_DIED,
} packet_type;

typedef struct __packet {
  packet_type type;
  char payload[PAYLOAD_MAX];
  size_t len; // payload data length
  int dest;
  // for CHILD_DIED
  pid_t child;
} packet_t;

typedef struct __atomic_jobq {
  int* pipe;
  pthread_mutex_t* write_lock;
} mpsc_t; // multple producer single consumer

mpsc_t* jobq_open(void);
void jobq_close(mpsc_t* queue);
packet_t jobq_recv(mpsc_t* queue);
void jobq_send(mpsc_t* queue, const packet_t pkt);
