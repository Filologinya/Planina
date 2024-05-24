#include <stdio.h>
#include <stdlib.h>

#include "Arguments.h"
#include "Hello.h"
#include "Logger.h"
#include "GraphicsEngine.h"

int main(int argc, char* argv[]) {
  Arguments args = parce_arguments(argc, argv);
  if (args.help > 0) {
    print_help();
    return 0;
  }

  init_logging(args.log_bufferisation, args.log_level);

  GraphicsEngine *engine = init_graphics_engine();
  if (engine == NULL) {
    plog_error("%s", "Can't init GraphicsEngine");
    exit(1);
  }

  engine_run(engine);

  free_graphics_engine(engine);

  plog_trace("end");
  return 0;
}
