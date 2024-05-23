#include "Logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int logging_level = LogLevelWarning;

char logging_buffer[2048];

// Log history for saving and writing trace logs on errors
typedef struct {
  char buffer[128][256];
  size_t index;
} HistBuffer;

HistBuffer hist_buffer;

void init_logging(int buffering, int log_level) {
  logging_level = log_level;
  if (buffering > 0)
    setvbuf(stdout, logging_buffer, _IOFBF, sizeof(logging_buffer));

  hist_buffer.index = 0;

  // TODO: Remove, it's just to show logs
  plog_error("error log testing");
  plog_warn("warning log testing");
  plog_info("info log testing");
  plog_debug("debug log testing");
  plog_trace("trace log testing");
}

void log_format(const char* filename, int linenum, const char* tag,
                const char* message, va_list args) {
  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  char buff[50];
  strftime(buff, sizeof buff, "%T", gmtime(&ts.tv_sec));
  printf("%s.%ld [%s] ", buff, ts.tv_nsec, tag);
  vprintf(message, args);
  printf("\t(%s:%d)\n", filename, linenum);
}

void log_lvl_func(const char* filename, int linenum, const char* log_level,
                  const char* message, ...) {
  va_list args;
  va_start(args, message);
  log_format(filename, linenum, log_level, message, args);
  va_end(args);
}

void log_error_func(const char* filename, int linenum, const char* message,
                    ...) {
  if (logging_level < LogLevelTrace) {
    trigger_logging_buffer();
  }
  va_list args;
  va_start(args, message);
  log_format(filename, linenum, "\033[91merror\033[39m", message, args);
  va_end(args);
}

void save_in_logging_buffer(const char* filename, int linenum,
                            const char* log_level, const char* message, ...) {
  size_t index = hist_buffer.index;

  memset(hist_buffer.buffer[index], 0, 256);

  struct timespec ts;
  timespec_get(&ts, TIME_UTC);
  char buff[50];
  strftime(buff, sizeof buff, "%T", gmtime(&ts.tv_sec));

  int offset = 0, curr_offset = 0;
  curr_offset =
      snprintf(hist_buffer.buffer[index] + offset, 256 - offset, "%s.%ld [%s] ",
               (const char*)buff, ts.tv_nsec, log_level);
  offset += curr_offset;
  va_list args;
  va_start(args, message);
  curr_offset = vsnprintf(hist_buffer.buffer[index] + offset, 256 - offset,
                          message, args);
  va_end(args);
  offset += curr_offset;
  snprintf(hist_buffer.buffer[index] + offset, 256 - offset, "\t(%s:%d)\n",
           filename, linenum);

  ++hist_buffer.index;
  if (hist_buffer.index >= 128) {
    hist_buffer.index = 0;
  }
}

void trigger_logging_buffer() {
  size_t index = hist_buffer.index;
  size_t i = index;
  if (index == 0) index = 127;
  --index;
  while (i != index) {
    if (hist_buffer.buffer[i][0] != 0) printf("%s", hist_buffer.buffer[i]);

    if (i >= 127)
      i = 0;
    else
      ++i;
  }
  if (hist_buffer.buffer[index][0] != 0)
    printf("%s", hist_buffer.buffer[index]);
}
