#include "stringf.h"
#include "core.h"
#include "state.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_TIME_SLEEP_MS 2000

void service_run(void) {
  if (state_init()) {
    for (;;) {
      state_update();
      tcp_signal();
      blink_once();
    }
  }

  system_die();
} 