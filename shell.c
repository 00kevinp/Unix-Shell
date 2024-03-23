#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "commands.h"
#include "error.h"
#include "shell.h"

#define MAX_CHAR_LIMIT 128
#define CMD_INDEX 0
#define MAX_PATHS 10
#define MAX_PATH_LENGTH 256
int pathFound = 0;
char paths[MAX_PATHS][MAX_PATH_LENGTH];
int num_paths = 0;

void initializePaths() {
    num_paths = 1;
    strcpy(paths[0], "/bin/");
}
// return 0 if command is built in, 1 if not
int identify_built_in_cmd(char **args, int num_args) {
  if (strncmp(args[CMD_INDEX], "exit", 4) == 0) {
    cmd_exit(args, num_args);
    return 0; // built-in command found
  } else if (strncmp(args[CMD_INDEX], "path", 4) == 0) {
    cmd_path(args, num_args);
    pathFound++;
    return 0; // built-in command found
  } else if (strncmp(args[CMD_INDEX], "cd", 2) == 0) {
    cmd_cd(args, num_args);
    return 0; // built-in command found
  }
  return 1; // command is not built-in
}

void remove_newline(char *text) {
  for (int i = 0; i < strlen(text); i++) {
    if (text[i] == '\n') {
      text[i] = '\0';
    }
  }
}

// stores multiple paths in an array
// and adds '/' to the end of each path
void cmd_path(char **args, int num_args) {
    if (num_args <= 1) {
        num_paths = 0;
    } else {
        num_paths = 0;
        for (int i = 1; i < num_args; i++) {
            strcpy(paths[num_paths], args[i]);
            int pathLength = strlen(paths[num_paths]);
            if (pathLength > 0 && paths[num_paths][pathLength - 1] != '/') {
                strcat(paths[num_paths], "/"); // adds '/' to the end of the path
            }
            num_paths++;
        }
    }
}

// if command cannot be found in internal files, search path list for it 
void execute_external_cmd(char **args, int num_args) {
    char cmd[MAX_CHAR_LIMIT];
    int fd = -1;
    int redirectOutput = 0;
    int redirectionCount = 0;
    FILE *outputFile = NULL;

    for (int i = 0; i < num_args; i++) {
      // treat redirection symbols
      if (strcmp(args[i], ">") == 0) {
        if (args[i+1] == NULL || args[i+2] != NULL) {
          error();
          return;
        }

        outputFile = fopen(args[i+1], "w");

        // make sure output file is valid
        if (outputFile == NULL) {
          error();
          return;
        }
        fd = fileno(outputFile);
        redirectOutput = 1;
        args[i] = NULL;
        break;
    }
  }

    // search through list of paths, finding the first
  // corresponding path to the command.
    for (int i = 0; i < num_paths; i++) {
        snprintf(cmd, sizeof(cmd), "%s%s", paths[i], args[0]);
        if (access(cmd, X_OK) == 0) {
            int rc = fork();
            if (rc < 0) {
                error();
                exit(0);
            } else if (rc == 0) { 
                if (redirectOutput) {
                    dup2(fd, STDOUT_FILENO);
                    dup2(fd, STDERR_FILENO);
                    close(fd);
                }
                execv(cmd, args);
                error(); 
                exit(0);
            } else {
                int wc = wait(NULL);
            }
            return; // cmd executed
        }
    }
    error(); // fatal error, path not found, command not found, etc.
}

// helper
int redirectFound(char c) {
  if (c == '>') {
    return 1;
  }
  else return 0;
}

// split the given arguments into separate pieces
int split_arguments(char *input, char **args) { // input is what will be tokenized, then added to args
    char *token = strtok(input, " ");
    int argument_count = 0;
    while (token != NULL) {
        char *rPointer = token;
        while (*rPointer != '\0') {
            if (redirectFound(*rPointer)) {
                if (rPointer != token) {
                    args[argument_count] = strndup(token, rPointer - token);
                    argument_count++;
                }
                args[argument_count] = strndup(rPointer, 1); // may have to change from 1 to larger int, not sure really???
                argument_count++;
                if (*(rPointer + 1) != '\0' && !redirectFound(*(rPointer + 1))) {
                    args[argument_count] = strdup(rPointer + 1); // chars after ">"
                    argument_count++;
                }
                break; // move to next token after handling redirection
            }
            rPointer++;
        }
        if (*rPointer == '\0') { // if redirection not found in token
            args[argument_count] = token;
            argument_count++;
        }
        token = strtok(NULL, " ");
    }
    args[argument_count] = NULL; // done... set to NULL to indicate the end of arguments list
    return argument_count;
}


// ./wish with no arguments
void interactive_mode() {
  printf("wish> ");

  char input[MAX_CHAR_LIMIT];
  fgets(input, MAX_CHAR_LIMIT, stdin);

  remove_newline(input);

  char *arguments[MAX_CHAR_LIMIT];
  int argument_count = split_arguments(input, arguments);
  if (identify_built_in_cmd(arguments, argument_count) == 1)
    execute_external_cmd(arguments, argument_count);
}

// ./wish <filename>
void batch_mode(char filename[]) {
  FILE *fp;
  char line[MAX_CHAR_LIMIT];

  fp = fopen(filename, "r"); // open file for reading
  if (fp == NULL) {
    perror("Error opening file");
  }
  if (fp != NULL && isFile(fp)) {
    while (fgets(line, MAX_CHAR_LIMIT, fp) != NULL) {
      printf("%s", line);
    }
  }
  while (fgets(line, MAX_CHAR_LIMIT, fp) != NULL) {

    remove_newline(line);
    // split up the string
    char *arguments[MAX_CHAR_LIMIT];
    int argument_count = split_arguments(line, arguments);

    if (identify_built_in_cmd(arguments, argument_count) == 1) {
      execute_external_cmd(arguments, argument_count);
    }
    // else {
    //   printf("%s", line);
    // }
  }
  fclose(fp);
}
