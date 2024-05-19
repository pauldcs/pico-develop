#include "pico/cyw43_arch.h"
#include "core.h"
#include <stdio.h>

# define WIFI_CONNECTION_TIMEOUT_MS 10000

bool wifi_is_connected(void) {
  return (cyw43_wifi_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_JOIN);
}

bool wifi_try_connect(void) {
  int pico_ret = cyw43_arch_wifi_connect_timeout_ms(
    WIFI_SSID,
    WIFI_PASS,
    CYW43_AUTH_WPA2_AES_PSK,
    WIFI_CONNECTION_TIMEOUT_MS
  );

  if (pico_ret != PICO_OK) {
    (void)fprintf(stderr,
      "%s: error: Could not establish a Wi-Fi connection (err: %d)\n",
      time_string(),
      pico_ret
    );
  
    return (false);
  }

  for(int i = 2; i; i--) {
    blink_once();
  }

  (void)fprintf(stderr,
    "%s: info: Wi-Fi connected\n", time_string()
  );

  return (true);
}

bool wifi_assure_connection(void) {
  if (wifi_is_connected()) {
    return (true);
  }

  for(int i = 5; i; i--) {
    blink_once();
  }

  (void)fprintf(stderr,
    "%s: info: Wi-Fi is not connected, attempting a connection to '%s'\n",
    time_string(),
    WIFI_SSID
  );

  return (wifi_try_connect());
}
