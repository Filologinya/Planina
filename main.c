#include <stdio.h>
#include <stdlib.h>

#include "Arguments.h"
#include "Hello.h"
#include "Logger.h"

int main(int argc, char* argv[]) {
  Arguments args = parce_arguments(argc, argv);
  if (args.help > 0) {
    print_help();
    return 0;
  }

  init_logging(args.log_bufferisation, args.log_level);
  Planina* planina = init_planina();
  if (planina == NULL) {
    plog_error("No allocated memory");
    exit(1);
  }

  planina_say_hello(planina);

  free_planina(planina);

  plog_trace("end");
  return 0;
}
