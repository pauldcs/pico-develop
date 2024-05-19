#include <stdio.h>
#include <sys/time.h>
#include <time.h>

# define TS_BUFFER_SIZE 24

const char *time_string(void)
{
  struct timeval tv;
	struct tm tm_time;
  static char ts[TS_BUFFER_SIZE];

  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &tm_time);
  strftime(ts, TS_BUFFER_SIZE, "%H:%M:%S", &tm_time);
  (void)snprintf(ts + 19, 5, ".%03d", (int)tv.tv_usec / 1000);
  return ts;
}
