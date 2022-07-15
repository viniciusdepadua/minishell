//
// Created by araujvin on 15/07/22.
//

#ifndef MINISHELL_PROCESS_H
#define MINISHELL_PROCESS_H

#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Process
{
    struct Process *next;
    int argc;
    char **argv;
    pid_t pid;
    char completed;
    char stopped;
    int status;

} Process;

#endif //MINISHELL_PROCESS_H
