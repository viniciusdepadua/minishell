#include "minishell.h"

int main() {
    init_shell();
    printf("Welcome to the miniature-shell.\n");
    while(true){
        Process *command;
        command = (Process*) malloc(sizeof(Process));

        char *command_input = readline("cmd> ");
        if(strcmp(command_input, "exit") == 0 || strcmp(command_input, "quit") == 0){
            free(command);
            free(command_input);
            break;
        }

        parse(command, command_input);
        free(command_input);

        //__debugToken(command);

        execute(command);

        freeCommands(command);
    }
    return 0;
}
