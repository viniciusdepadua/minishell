//
// Created by araujvin on 25/04/22.
//

#ifndef MINISHELL_MINISHELL_H
#define MINISHELL_MINISHELL_H

#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>
#include "process.h"
#include "job.h"
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#define CAP_TOKENS 10

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void init_shell();
void parse(Process *c, char * ci);
int execute(Process *c);
int spawnProcess(int in, int out, Process *c);
int outCommand(int in, Process *c);
int inCommand(int out, Process *c);
int lastCommand(int in, Process *c);
void freeCommands(Process *c);
void __debugToken(Process *c);

#endif //MINISHELL_MINISHELL_H
