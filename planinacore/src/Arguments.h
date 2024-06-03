#pragma once

typedef struct {
  int log_level;
  int log_bufferisation;
  int help;
} Arguments;

Arguments parce_arguments(int argc, char* argv[]);

void print_help();