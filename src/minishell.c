//
// Created by araujvin on 25/04/22.
//
#include "minishell.h"

void parse(Command *c, char * ci){
    char *token = strtok(ci, " ");
    int sz = CAP_TOKENS;
    c->argv = (char **) calloc(sz, sizeof(char*));
    int args = 0;
    Command *itr = c;
    while( token != NULL ) {
        // "cmd1" -> pipe -> "cmd2"
        if(strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, "|") == 0){
            // ->pipe
            itr->next = (Command*) malloc(sizeof(Command));
            itr->argc = args;
            args = 0;

            itr = itr->next;
            itr->argv = (char **) calloc(1, sizeof(char*));
            itr->argv[args] = (char*) malloc(2);
            strcpy(itr->argv[args], token);
            itr->argc = 1;
            // -> "cmd2"
            itr->next = (Command*) calloc(1, sizeof(Command));
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

int outCommand(int in, Command *b){
    Command *c = b->next->next;
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
            dup2 (in, 0);
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

int inCommand(int out, Command*c){
    printf("entered in command\n");
    return 0;
}

int spawnProcess(int in, int out, Command*c){
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

int lastCommand(int in, Command* c){
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

int execute(Command *c){
    int in = STDIN_FILENO;
    int fd[2];

    Command * curr = c;
    Command * bef = NULL;

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
            if (inCommand(fd[1], curr) == -1){
                return EXIT_FAILURE;
            }
            curr = curr->next->next;
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

void freeCommands(Command *c){
    while (c != NULL){
        Command *temp = c;
        c = c -> next;
        for(int i = 0 ; i < temp->argc; i ++){
            free(temp->argv[i]);
        }
        free(temp->argv);
        free(temp);
    }
}

void __debugToken(Command *c){
    Command *itr = c;
    int j = 0;
    while(itr != NULL){
        for(int i = 0; i < itr->argc; i++){
            printf("token %d, arg %d: %s\n", j, i, itr->argv[i]);
        }
        itr = itr->next;
    }
}