//
// Created by araujvin on 25/04/22.
//
#include "minishell.h"

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

void init_shell(){
    setpgid(getpid(), getpid());
    tcsetpgrp (STDIN_FILENO, getpid());
    signal (SIGINT, SIG_DFL);
    signal (SIGQUIT, SIG_DFL);
    signal (SIGTSTP, SIG_DFL);
    signal (SIGTTIN, SIG_DFL);
    signal (SIGTTOU, SIG_DFL);
    signal (SIGCHLD, SIG_DFL);
}

void parse(Process *c, char * ci){
    char *token = strtok(ci, " ");
    int sz = CAP_TOKENS;
    c->argv = (char **) calloc(sz, sizeof(char*));
    int args = 0;
    Process *itr = c;
    while( token != NULL ) {
        // "cmd1" -> pipe -> "cmd2"
        if(strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, "|") == 0){
            // ->pipe
            itr->next = (Process*) malloc(sizeof(Process));
            itr->argc = args;
            args = 0;

            itr = itr->next;
            itr->argv = (char **) calloc(1, sizeof(char*));
            itr->argv[args] = (char*) malloc(2);
            strcpy(itr->argv[args], token);
            itr->argc = 1;
            // -> "cmd2"
            itr->next = (Process*) calloc(1, sizeof(Process));
            itr = itr->next;

            itr->argc = 0;
            itr->argv = (char **) calloc(sz, sizeof(char*));
            itr->next = NULL;
            sz = CAP_TOKENS;
        }
        else{
            if(args == sz){
                // build 2 * sz if number of arguments is bigger than initial threshold
                itr->argv = (char **) realloc(itr->argv, 2 * sz *sizeof (char*));
                sz = 2 * sz;
            }
            itr->argv[args] = (char*) malloc(strlen(token) + 1);
            strcpy(itr->argv[args],token);
            args++;
        }
        token = strtok(NULL, " ");
    }
    itr->argc = args;
    itr->next = NULL;
}

int outCommand(int in, Process *b){
    Process *c = b->next->next;
    int std_out = dup(STDOUT_FILENO);
    int fout = open(c->argv[0], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) {
        perror("Something wrong with file, ERROR");
        return -1;
    }
    if(dup2(fout, STDOUT_FILENO) == -1){
        perror("ERROR: dup2(2)");
        return EXIT_FAILURE;
    }
    if(fork() == 0){
        // child
        if (in != STDIN_FILENO)
        {
            dup2 (in, STDIN_FILENO);
            close (in);
        }
        close(fout);
        execve(b->argv[0], b->argv, 0);
        perror("Execution error");
        return 0;
    }
    // parent
    dup2(std_out, STDOUT_FILENO);
    close(fout);
    wait(NULL);
    return 0;
}

int inCommand(int out, Process *b){
    Process *c = b->next->next;
    int std_in = dup(STDIN_FILENO);
    int fin = open(c->argv[0], O_RDONLY);
    if (fin < 0) {
        perror("Something wrong with file, ERROR");
        return -1;
    }
    if(dup2(fin, STDIN_FILENO) == -1){
        perror("ERROR: dup2(2)");
        return EXIT_FAILURE;
    }
    if(fork() == 0){
        // child
        if (out != STDOUT_FILENO)
        {
            dup2 (out, STDOUT_FILENO);
            close (out);
        }
        close(fin);
        execve(b->argv[0], b->argv, 0);
        perror("Execution error");
        return 0;
    }
    // parent
    dup2(std_in, STDIN_FILENO);
    close(fin);
    wait(NULL);
    return 0;
}


int mark_process_status (pid_t pid, int status)
{
    Job *j;
    Process *p;

    if (pid > 0)
    {
        /* Update the record for the process.  */
        for (j = first_job; j; j = j->next)
            for (p = j->first_process; p; p = p->next)
                if (p->pid == pid){
                    p->status = status;
                    if (WIFSTOPPED (status))
                        p->stopped = 1;
                    else{
                        p->completed = 1;
                        if (WIFSIGNALED (status))
                            fprintf (stderr, "%d: Terminated by signal %d.\n",
                                     (int) pid, WTERMSIG (p->status));
                    }
                    return 0;
                }
        fprintf (stderr, "No child process %d.\n", pid);
        return -1;
    }

    else if (pid == 0 || errno == ECHILD)
        /* No processes ready to report.  */
        return -1;
    else {
        /* Other weird errors.  */
        perror ("waitpid");
        return -1;
    }
}


/* Check for processes that have status information available,
   without blocking.  */

void update_status (void)
{
    int status;
    pid_t pid;

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED|WNOHANG);
    while (!mark_process_status (pid, status));
}


/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */

void wait_for_job (Job *j)
{
    int status;
    pid_t pid;

    do
        pid = waitpid (WAIT_ANY, &status, WUNTRACED);
    while (!mark_process_status (pid, status)
           && !job_is_stopped (j)
           && !job_is_completed (j));
}


/* Format information about job status for the user to look at.  */

void format_job_info (Job *j, const char *status)
{
    fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}


/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */

void do_job_notification (void)
{
    Job *j, *jlast, *jnext;

    /* Update status information for child processes.  */
    update_status ();

    jlast = NULL;
    for (j = first_job; j; j = jnext)
    {
        jnext = j->next;

        /* If all processes have completed, tell the user the job has
           completed and delete it from the list of active jobs.  */
        if (job_is_completed (j)) {
            format_job_info (j, "completed");
            if (jlast)
                jlast->next = jnext;
            else
                first_job = jnext;
        }

            /* Notify the user about stopped jobs,
               marking them so that we won’t do this more than once.  */
        else if (job_is_stopped (j) && !j->notified) {
            format_job_info (j, "stopped");
            j->notified = 1;
            jlast = j;
        }

            /* Don’t say anything about jobs that are still running.  */
        else
            jlast = j;
    }
}


void put_job_in_foreground (Job *j, int cont){
    /* Put the job into the foreground.  */
    tcsetpgrp (shell_terminal, j->pgid);


    /* Send the job a continue signal, if necessary.  */
    if (cont){
        tcsetattr (shell_terminal, TCSADRAIN, &j->tmodes);
        if (kill (- j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
    }


    /* Wait for it to report.  */
    wait_for_job (j);

    /* Put the shell back in the foreground.  */
    tcsetpgrp (shell_terminal, shell_pgid);

    /* Restore the shell’s terminal modes.  */
    tcgetattr (shell_terminal, &j->tmodes);
    tcsetattr (shell_terminal, TCSADRAIN, &shell_tmodes);
}

void put_job_in_background (Job *j, int cont){
    /* Send the job a continue signal, if necessary.  */
    if (cont)
        if (kill (-j->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
}

int spawnProcess(int in, int out, Process*c){
    pid_t pid;
    pid = fork();
    if(pid == -1){
        perror("fork");
        return -1;
    }
    else if (pid == 0)
    {
        //child
        if (in != STDIN_FILENO){
            dup2 (in, STDIN_FILENO);
            close (in);
        }

        if (out != STDOUT_FILENO){
            dup2 (out, STDOUT_FILENO);
            close (out);
        }

        execve (c->argv [0], c->argv, 0);
        perror("Execution error");
    }
    wait(NULL);
    return pid;
}

int lastCommand(int in, Process* c){
    pid_t status_pid;
    int pid = fork();
    if(pid < 0){
        perror("Fork()");
        return -1;
    }
    else if(pid == 0){
        if(in != STDIN_FILENO){
            dup2(in, STDIN_FILENO);
            close(in);
        }
        execve(c->argv[0], c->argv, 0);
        perror("Execution error");
        return 1;
    }
    wait(NULL);
    return 0;
}

int execute(Process *c){
    int in = STDIN_FILENO;
    int fd[2];

    Process * curr = c;
    Process * bef = NULL;

    while(curr != NULL && curr ->next != NULL) {
        if (pipe(fd) == -1){
            perror("pipe");
            return EXIT_FAILURE;
        }
        if(strcmp(curr->next->argv[0], "|") == 0){
            if(spawnProcess(in, fd[1], curr) == -1){
                return EXIT_FAILURE;
            }
            curr = curr->next;
        } else if(strcmp(curr->next->argv[0], ">") == 0){
            if(outCommand(in, curr) == -1){
                return EXIT_FAILURE;
            }
            curr = curr->next->next;
        } else if(strcmp(curr->next->argv[0], "<") == 0){
            int out = curr->next->next->next == NULL? STDOUT_FILENO : fd[1];
            if (inCommand(out, curr) == -1){
                return EXIT_FAILURE;
            }
            curr = curr->next->next->next == NULL? curr->next->next: curr->next->next->next;
        }
        curr = curr->next;
        close(fd[1]);
        in = fd[0];
    }

    if (curr == NULL){
        return 0;
    }
    if(lastCommand(in, curr) == -1){
        return EXIT_FAILURE;
    }
    return 0;

}

void freeCommands(Process *c){
    while (c != NULL){
        Process *temp = c;
        c = c -> next;
        for(int i = 0 ; i < temp->argc; i ++){
            free(temp->argv[i]);
        }
        free(temp->argv);
        free(temp);
    }
}

void __debugToken(Process *c){
    Process *itr = c;
    int j = 0;
    while(itr != NULL){
        for(int i = 0; i < itr->argc; i++){
            printf("token %d, arg %d: %s\n", j, i, itr->argv[i]);
        }
        itr = itr->next;
    }
}