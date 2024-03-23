#include "commands.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "error.h"

#define EXIT_SUCCESS 0
#define EXIT_FAIL 1
#define MAX_LIMIT 128

char path[MAX_LIMIT]; //default path is /bin
char *defaultPath = DEFAULT_PATH;

// return 0 if string length < 2, return 1 otherwise
int str_is_empty(char *str) {
  if (strlen(str) < 2)
    return 0;
  return 1;
}

void cmd_exit(char **text, int num_args) {
  if (num_args > 1) {
    if (str_is_empty(text[1]) == 0)
      exit(0);
    error();
  } else {
    exit(0);
  }
}

void cmd_cd(char **args, int num_args) {
  if (chdir(args[1]) != 0)
    error();
}

int isFile(char *pth) {
    struct stat path_stat;
    stat(pth, &path_stat);
    return S_ISREG(path_stat.st_mode);
}
