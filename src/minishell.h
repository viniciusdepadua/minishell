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

void init_shell();
void parse(Process *c, char * ci);
int execute(Process *c);
void put_job_in_background (Job*j, int cont);
void put_job_in_foreground (Job *j, int cont);
int mark_process_status(pid_t pid, int status);
void update_status();
void wait_for_job (Job *j);
void format_job_info (Job *j, const char *status);
void do_job_notification();
int spawnProcess(int in, int out, Process *c);
int outCommand(int in, Process *c);
int inCommand(int out, Process *c);
int lastCommand(int in, Process *c);
void freeCommands(Process *c);
void __debugToken(Process *c);

#endif //MINISHELL_MINISHELL_H
