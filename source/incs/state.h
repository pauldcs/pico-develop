#ifndef __STATE_H__
# define __STATE_H__

# include "pico/unique_id.h"
# include <stdbool.h>

typedef struct {
  uint8_t id[PICO_UNIQUE_BOARD_ID_SIZE_BYTES];
  bool is_reachable;
} state_t;

extern state_t *g_state;
extern int blink_speed;

bool state_init(void);
void state_update(void);
void state_print(void);
char  *state_get(void);

bool tcp_signal(void);

#endif /* __STATE_H__ */