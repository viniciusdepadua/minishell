//
// Created by araujvin on 15/07/22.
//

#ifndef MINISHELL_JOB_H
#define MINISHELL_JOB_H
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <termios.h>
#include "process.h"

typedef struct Job
{
    struct Job *next;           /* next active job */
    char *command;              /* command line, used for messages */
    Process *first_process;     /* list of processes in this job */
    pid_t pgid;                 /* process group ID */
    char notified;              /* true if user told about stopped job */
    struct termios tmodes;      /* saved terminal modes */
    int stdin, stdout, stderr;  /* standard i/o channels */
} Job;

Job* first_job;
Job * find_job(pid_t pgid);
int job_is_stopped(Job* j);
int job_is_completed(Job* j);

#endif //MINISHELL_JOB_H
