#pragma once

// Initialization:
//
// Call init_logging with buffering = 0 and log_level = 1 as a default values
// LogLevel could be:
// 0 - LogLevelDisabled - Disabled, no logging and assert messages
// 1 - LogLevelError - Error messages, they will call trace and debug messages
// on errors 2 - LogLevelWarning - Warning on some suspicious info, which won't
// perturb programm execution 3 - LogLevelInfo - Info messages, just versions
// and additional information about modules, etc 4 - LogLevelDebug - Debug
// messages, information about time, resource usage and checking variables 5 -
// LogLevelTrace - Trace messages, can log every call or if statement to check
// programm trace
//
// Usage:
//
// plog_<log_level>(<format_string>, arguments);
// plog_error
// plog_warning
// plog_info
// plog_debug
// plog_trace
//
// for example:
// plog_debug("Got window [%p] with size (%d, %d)", window_ptr, width, height);

extern int logging_level;

// see https://searchfox.org/comm-central/source/mozilla/xpcom/base/Logging.h#62
enum LogLevel {
  LogLevelDisabled = 0,
  LogLevelError,
  LogLevelWarning,
  LogLevelInfo,
  LogLevelDebug,
  LogLevelTrace,
};

void init_logging(int buffering, int log_level);

// Internal part:
void log_lvl_func(const char* filename, int linenum, const char* log_level,
                  const char* message, ...);

void save_in_logging_buffer(const char* filename, int linenum,
                            const char* log_level, const char* message, ...);
void trigger_logging_buffer();

void log_error_func(const char* filename, int linenum, const char* message,
                    ...);

#define plog_error(...)               \
  if (logging_level >= LogLevelError) \
  log_error_func(__FILE__, __LINE__, __VA_ARGS__)

#define plog_warn(...)                  \
  if (logging_level >= LogLevelWarning) \
  log_lvl_func(__FILE__, __LINE__, "\033[93mwarn\033[39m", __VA_ARGS__)

#define plog_info(...)               \
  if (logging_level >= LogLevelInfo) \
  log_lvl_func(__FILE__, __LINE__, "\033[96minfo\033[39m", __VA_ARGS__)

#define plog_debug(...)                                                       \
  do {                                                                        \
    if (logging_level >= LogLevelDebug)                                       \
      log_lvl_func(__FILE__, __LINE__, "\033[32mdebug\033[39m", __VA_ARGS__); \
    else                                                                      \
      save_in_logging_buffer(__FILE__, __LINE__, "\033[32mdebug\033[39m",     \
                             __VA_ARGS__);                                    \
  } while (0);

#define plog_trace(...)                                                 \
  do {                                                                  \
    if (logging_level >= LogLevelTrace)                                 \
      log_lvl_func(__FILE__, __LINE__, "trace", __VA_ARGS__);           \
    else                                                                \
      save_in_logging_buffer(__FILE__, __LINE__, "trace", __VA_ARGS__); \
  } while (0);

#define plassert(expr)                           \
  do {                                           \
    if (expr) {                                  \
      plog_trace("Assertion passed: %s", #expr); \
    } else {                                     \
      plog_error("Assertion failed: %s", #expr); \
      exit(1);                                   \
    }                                            \
  } while (0);
