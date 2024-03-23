/*
Original Author: Austin Pliska
Traded with: Kevin Pickelman
*/

#include "shell.h"
#include "commands.h"
#include "error.h"

#define NO_ARGUMENTS 1

int main(int argc, char *argv[]) {
  initializePaths();
  while (argc == NO_ARGUMENTS)
    interactive_mode();
  if (argc > NO_ARGUMENTS && argc < 3 && isFile(argv[1])) {
    batch_mode(argv[1]);
  }

  else {
    error();
  }

  return 0;
}
