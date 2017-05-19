#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "session.h"

#define NAME "multty"
#define VERSION "alpha"


void usage(void) {
  fprintf(stderr, "usage: "NAME" [-v]\n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  int opt;
  while ((opt = getopt(argc, argv, "v")) != -1) {
    switch (opt) {
      case 'v':
        puts(NAME " " VERSION);
        exit(EXIT_SUCCESS);
      default:
        usage();
    }
  }

  session_start();
}
