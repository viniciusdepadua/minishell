//
// Created by araujvin on 25/04/22.
//
#include "minishell.h"

void parse(Command *c, char * ci){
    char *token = strtok(ci, " ");
    int sz = CAP_TOKENS;
    c->argv = (char **) calloc(sz, sizeof(char));
    int args = 0;
    Command *itr = c;
    while( token != NULL ) {
        if(args == sz){
            char ** tokens = (char **) realloc(itr->argv, 2 * sz);
            memset(tokens + sz, 0, sz);
            itr->argv = tokens;
            sz = 2 * sz;
        }
        if(strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, "|") == 0){
            itr->next = (Command*) malloc(sizeof(Command));
            itr->argc = args;
            args = 0;
            itr = itr->next;
            itr->argv = (char **) calloc(sz, sizeof(char));
            itr->argv[args] = token;
            itr->argc = 1;
            itr->next = (Command*) calloc(1, sizeof(Command));
            itr = itr->next;
            itr->argc = 0;
            itr->argv = (char **) calloc(sz, sizeof(char));
        }
        else{
            itr->argv[args] = token;
            args++;
        }
        token = strtok(NULL, " ");
    }
    itr->argc = args;
    free(token);
}

int outCommand(Command *b, Command*c){
    int std_out = dup(STDOUT_FILENO);
    int fout = open(c->argv[0], O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fout < 0) {
        perror("Something wrong with file, ERROR");
        return 1;
    }
    if(dup2(fout, STDOUT_FILENO) == -1){
        perror("ERROR: dup2(2)");
        return EXIT_FAILURE;
    }
    if(fork() == 0){
        // child
        execve(b->argv[0], b->argv, 0);
        close(fout);
        return 0;
    }
    // parent
    dup2(std_out, STDOUT_FILENO);
    close(fout);
    wait(NULL);
    return 0;
}

int inCommand(Command *b, Command*c){
    printf("entered in command\n");
    return 0;
}

int pipeCommand(Command *b, Command*c){
    return 0;
}

int n_command(Command* c){
    pid_t status_pid;
    int status;
    int pid = fork();
    if(pid < 0){
        perror("Fork()");
        return 1;
    }
    else if(pid == 0){
        execve(c->argv[0], c->argv, 0);
        return 1;
    }
    else{
        do {
            status_pid = wait(&status);
            if (status_pid == -1) {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        return 0;
    }
}

int process(Command *command){
    Command * curr = command;
    Command * bef = NULL;
    while(curr != NULL) {
        bef = curr;
        curr = curr->next;
        if(curr == NULL){
            n_command(bef);
        } else if(strcmp(curr->argv[0], ">") == 0 ){
            curr = curr->next;
            outCommand(bef, curr);
            curr = curr->next;
        }
        else if(strcmp(curr->argv[0], "<") == 0){
            curr = curr->next;
            outCommand(bef, curr);
            curr = curr->next;
        }
        else if(strcmp(curr->argv[0], "|") == 0){
            curr = curr->next;
            pipeCommand(bef, curr);
            curr = curr->next;
        }
    }
    return 0;
}

void freeCommands(Command *c){
    while (c != NULL){
        Command *temp = c;
        c = c -> next;
        temp->next = NULL;
        free(temp->argv);
        temp->argv = NULL;
        free(temp);
        temp = NULL;
    }
}

void __debugToken(Command *c){
    Command *itr = c;
    while(itr != NULL){
        for(int i = 0; i < itr->argc; i++){
            printf("token %d: %s\n", i, itr->argv[i]);
        }
        itr = itr->next;
    }
}