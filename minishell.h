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
int process(Command *c);
int outCommand(Command *b, Command *c);
int inCommand(Command *b, Command *c);
int n_Command(Command *c);
void freeCommands(Command *c);
void __debugToken(Command *c);
#endif //MINISHELL_MINISHELL_H
