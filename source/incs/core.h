#ifndef __CORE_H__
# define __CORE_H__

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

const char *time_string(void);
/*
 *    Goes in an infinite blinking loop.
 */
void system_die(void);

void set_blink_success(void);
void set_blink_error(void);
void set_blink_waiting(void);
void blink_twice(void);

void core1_entry(void);

/*
 *    Main program loop.
 */
void service_run(void);

bool wifi_try_connect(void);
bool wifi_is_connected(void);
bool wifi_assure_connection(void);

#endif /* __CORE_H__ */