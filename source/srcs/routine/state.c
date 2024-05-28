#include "state.h"
#include "pico/unique_id.h"
#include <string.h>
#include "stringf.h"
#include "core.h"
#include <stdio.h>
#include <stdint.h>

bool state_init(void) {
  (void)memset(g_state, 0x00, sizeof(state_t));
  pico_get_unique_board_id(g_state->id);
  return (true);
}

void state_update(void) {
  g_state->is_reachable = wifi_assure_connection();
  if (g_state->is_reachable) {
    set_blink_success();
  } else {
    set_blink_waiting();
  }
}

void state_print(void) {
  char id_buf[32];
  (void)slcpyf(
    id_buf,
    32, "\\x%x\\x%x\\x%x\\x%x\\x%x\\x%x\\x%x\\x%x",
    g_state->id[0], g_state->id[1],
    g_state->id[2], g_state->id[3],
    g_state->id[4], g_state->id[5],
    g_state->id[6], g_state->id[7]
  );

  (void)fprintf(stderr,
    "state: {\n"
    "  id: '%s'\n"
    "  is_reachable: %s\n"
    "}\n",
    id_buf,
    g_state->is_reachable ? "yes" : "no"
  );
}

char  *state_get(void) {
  // static char state[512];

  static char id_buf[64];
  (void)slcpyf(
    id_buf,
    32, "\\x%x\\x%x\\x%x\\x%x\\x%x\\x%x\\x%x\\x%x",
    g_state->id[0], g_state->id[1],
    g_state->id[2], g_state->id[3],
    g_state->id[4], g_state->id[5],
    g_state->id[6], g_state->id[7]
  );

  // size_t size = slcpyf(
  //   state,
  //   512,
  //   "state: {\n"
  //   "  id: '%s'\n"
  //   "  is_reachable: %s\n"
  //   "}\n",
  //   id_buf,
  //   g_state->is_reachable ? "yes" : "no"
  // );

  id_buf[33] = 0;
  return id_buf;
}