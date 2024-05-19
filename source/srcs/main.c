#include "core.h"
#include "array.h"
#include "core.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "stringf.h"
#include "state.h"
#include <stdbool.h>

array_allocator_t __array_allocator__ = {
    ._memory_alloc = malloc,
    ._memory_realloc = realloc,
	._memory_free = free
};

state_t  __state__;
state_t *g_state = &__state__;

void system_die(void) {
    bool on = true;

    (void)fprintf(stderr, "%s: System error\n", time_string());
  
    while (true) {
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, on = !on);
      sleep_ms(50);
    }
  
    __builtin_unreachable();
}

static inline void service_init(void) {
    if (stdio_init_all() == 0 || cyw43_arch_init() != 0) {
        (void)fprintf(stderr,
            "error: system library initialisation failure\n"
        );
    
        system_die();
    }

    cyw43_arch_enable_sta_mode();
}

static inline void service_deinit(void) {
    cyw43_arch_deinit();
}

__attribute__(())

void blink_once(void) {
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
  sleep_ms(50);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
  sleep_ms(50);
}

int main(void) {
    service_init();
    service_run();
    service_deinit();
}