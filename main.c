#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <unistd.h>

#define CAP_TOKENS 32

typedef struct Command
{
    char *name;
    int argc;
    char **argv;
} Command;

Command command;

void parse(Command *c, char * ci){
    char *token = strtok(ci, " ");
    int sz = CAP_TOKENS;
    c->argv = (char **) calloc(sz, sizeof(char));
    int args = 0;
    while( token != NULL ) {
        if(args == sz){
            char ** tokens = (char **) realloc(c->argv, 2 * sz);
            memset(tokens + sz, 0, sz);
            c->argv = tokens;
            sz = 2 * sz;
        }
        c->argv[args] = token;
        token = strtok(NULL, " ");
        args++;
    }
    free(token);
    c->argc = args;
}

int outCommand(){
    printf("entered out command\n");
    return 0;
}


int inCommand(){
    printf("entered in command\n");
    return 0;
}


int pipeCommand(){
    printf("entered pipe command\n");
    return -1;
}

int n_command(){
    printf("entered normal command\n");
    return 127;
}

int process(){
    int std_in = dup(0);
    int std_out = dup(1);
    int std_err = dup(2);
    for(int i = 1; i < command.argc; i++){
        if(strcmp(command.argv[i], ">") == 0){
            if(outCommand() != 0){
                perror("error in execution");
                return -1;  
            };
        }
        else if(strcmp(command.argv[i], "<") == 0){
            if(inCommand() != 0){
                perror("error in execution");
                return -1;
            };
        }
        else if(strcmp(command.argv[i], "|") == 0){
            if(pipeCommand() != 0){
                perror("error in execution");
                return -1;
            };
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    printf("Welcome to the miniature-shell.\n");
    while(true){
        char *command_input = readline("cmd> ");
        if(strcmp(command_input, "exit") == 0 || strcmp(command_input, "quit") == 0){
            break;
        }

        parse(&command, command_input);

        /*for(int i = 0; i < command.argc; i++){
            printf("token %d: %s\n", i, command.argv[i]);
        }*/

        process();
        free(command.name);
        free(command.argv);
    }
    return 0;
}
