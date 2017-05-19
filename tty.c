#include "tty.h"

tty_t tty_spawn(void) {
  tty_t result = {
    .master_fd = -1,
    .pid = -1,
  };

  return result;

}
