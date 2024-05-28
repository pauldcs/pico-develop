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
    for (;; sleep_ms(DEFAULT_TIME_SLEEP_MS)) {
      state_update();
      if (tcp_signal()) {
        blink_twice();
      } else {
        sleep_ms(DEFAULT_TIME_SLEEP_MS);
      }
    }
  }

  system_die();
} 