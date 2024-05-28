#include "pico/cyw43_arch.h"
#include "pico/multicore.h"

int blink_speed = 1000;
int wait_time = 1000;

void set_blink_error(void) {
  blink_speed = 10;
  wait_time = 10;
}

void set_blink_waiting(void) {
  blink_speed = 1000;
  wait_time = 1000;
}

void set_blink_success(void) {
  blink_speed = 50;
  wait_time = 4000;
}

void core1_entry(void) {
    while (1) {
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
      sleep_ms(blink_speed);
      cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
      sleep_ms(wait_time);
    }
}

void blink_twice(void) {
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
  sleep_ms(30);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
  sleep_ms(30);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
  sleep_ms(30);
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
  sleep_ms(30);
}
