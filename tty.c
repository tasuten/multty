#include "tty.h"

tty_t tty_spawn(void) {
  tty_t result = {
    .master_fd = -1,
    .pid = -1,
  };

  result.pid = forkpty(&result.master_fd, NULL, NULL, NULL);
  if ( result.pid == -1 ) {
    fprintf(stderr, "forkpty() dead with errno: %d\n", errno);
    exit(EXIT_FAILURE);
  }
  return result;
}
