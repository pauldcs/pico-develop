#ifndef __CORE_H__
# define __CORE_H__

# include <stdbool.h>
# include <stdint.h>
# include <stddef.h>
# include <sys/types.h>

const char *time_string(void);
ssize_t hexdump(const uint8_t *addr, size_t n, size_t offset);
/*
 *    Goes in an infinite blinking loop.
 */
void system_die(void);
void blink_once(void);

/*
 *    Main program loop.
 */
void service_run(void);

bool wifi_try_connect(void);
bool wifi_is_connected(void);
bool wifi_assure_connection(void);

#endif /* __CORE_H__ */