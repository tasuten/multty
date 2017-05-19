#include "tty.h"

tty_t tty_spawn(void) {
  tty_t result = {
    .master_fd = -1,
    .pid = -1,
  };

  result.pid = forkpty(&result.master_fd, NULL, NULL, NULL);
  if ( result.pid == -1 ) {
    fprintf(stderr, "forkpty() died with error: %s\n", strerror(errno));
    exit(EXIT_FAILURE);
  }
  return result;
}

void tty_shell(const tty_t self, const char* shell) {
  if (self.pid == 0) {
    // child
    extern char **environ;
    if ( execle(shell, (char *)NULL, environ) == -1) {
      fprintf(stderr, "exec() died with error: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  } else {
    // parent
  }

}
