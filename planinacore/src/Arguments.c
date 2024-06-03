#include "Arguments.h"

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Logger.h"

Arguments parce_arguments(int argc, char* argv[]) {
  Arguments ans;
  memset(&ans, 0, sizeof(Arguments));
  ans.log_level = LogLevelError;

  int rez = 0;
  while ((rez = getopt(argc, argv, "hvpl:b:")) != -1) {
    switch (rez) {
      case 'h':
        ans.help = 1;
        break;
      case 'v':
        ans.log_level = LogLevelInfo;
        break;
      case 'p':
        ans.help = 1;
        break;
      case 'l':
        ans.log_level = atoi(optarg);
        break;
      case 'b':
        ans.log_bufferisation = atoi(optarg);
        break;
      case '?':
        if (optopt == 'l')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (optopt == 'b')
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint(optopt))
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        ans.help = 1;
        break;
    }
  }
  return ans;
}

void print_help() {
  const char* help_msg =
      "\n"
      "arguments:\n"
      "  -h        print help message\n"
      "  -v        set logging level to Info\n"
      "  -l <num>  set logging level (1 - default, from 0 to 5)\n"
      "  -b <num>  set logging bufferisation (0 - default, 1 to activate io "
      "buffer)\n"
      "\n"
      "logging levels: (are tooken from "
      "https://searchfox.org/comm-central/source/mozilla/xpcom/base/"
      "Logging.h#62)\n"
      "  0 - Disabled, no error messages, just asserts\n"
      "  1 - Error messages, they will call trace and debug messages on "
      "errors\n"
      "  2 - Warning messages about strange info which doesn't broke "
      "execution\n"
      "  3 - Info messages, just versions and additional information about "
      "modules, etc\n"
      "  4 - Debug messages, information about time, resource usage and "
      "checking "
      "variables\n"
      "  5 - Trace messages, can log every call or if statement to check "
      "programm "
      "trace\n";
  printf("Planina (v%s) voxels graphics engine\n%s\n", "0.0.1", help_msg);
}