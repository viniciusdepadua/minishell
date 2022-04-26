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

#define CAP_TOKENS 10

typedef struct Command
{
    struct Command *next;
    int argc;
    char **argv;

} Command;

void parse(Command *c, char * ci);
int execute(Command *c);
int spawnProcess(int in, int out, Command *c);
int outCommand(int in, Command *c);
int inCommand(int out, Command *c);
int lastCommand(int in, Command *c);
void freeCommands(Command *c);
void __debugToken(Command *c);

#endif //MINISHELL_MINISHELL_H
