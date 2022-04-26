#include "minishell.h"

int main() {
    printf("Welcome to the miniature-shell.\n");
    while(true){
        Command *command;
        command = (Command*) malloc(sizeof(Command));
        char *command_input = readline("cmd> ");
        if(strcmp(command_input, "exit") == 0 || strcmp(command_input, "quit") == 0){
            break;
        }
        parse(command, command_input);
        process(command);
    }
    return 0;
}
