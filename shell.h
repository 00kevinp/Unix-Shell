#ifndef SHELL_H
#define SHELL_H

int split_arguments(char *input, char **args);
void interactive_mode();
void batch_mode(char filename[]);
int identify_built_in_cmd(char **args, int num_args);
void execute_external_cmd(char **args, int num_args);
void cmd_path(char **text, int num_args);
char *return_path();
void initializePaths();

#endif
