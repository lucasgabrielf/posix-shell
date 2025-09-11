#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 16 // maximum number of arguments passed
#define MAX_COMMANDS 8

const char *commands[] = {
    "pwd",
    "cd",
    "ls",
    "echo",
    "exit",
    "cat",
    "touch",
    "rm",
    "cp",
    "grep",
    NULL
};

/*
! COMMAND PARSING SECTION
*/
// struct to store a command written in the shell (method + *args)
typedef struct {
    // method have to be in a list of valid commands
    char *method;
    char *args[MAX_ARGS];
    int argc;
    int is_valid;
} Command;

int isspace(int c) {
    switch (c) {
        case ' ':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            return 1; // is a whitespace character
        default:
            return 0; // is not
    }
}

// ! Deprecated method, as it does not account for parallel execution of multiple commands
// Command parse_command(char *line, const char **commands){
//     // parser created to populate the scruct with the correct method and arguments list
//     Command result = {NULL, NULL, 0};
//     result.argc = 0;
//     char *ptr = line;

//     while (*ptr && isspace((unsigned char)*ptr)) {
//         ptr++;
//     }
    
//     if (*ptr == '\0' or *ptr == '&') {
//         *ptr = '\0';
//         return result;
//     }

//     result.method = ptr;

//     while (*ptr && !isspace((unsigned char)*ptr)) {
//         ptr++;
//     }

//     *ptr = '\0';
//     ptr++;

//     while (*ptr != '\0' && result.argc < MAX_ARGS - 1) {
//         // skp white spaces and if null terminaded, finish
//         while (*ptr && isspace((unsigned char)*ptr)) {
//             ptr++;
//         }

//         if (*ptr == '\0') {
//             break;
//         }

//         // saves the pointer to the start of new arg
//         result.args[result.argc] = ptr;
//         result.argc++;

//         while (*ptr && !isspace((unsigned char)*ptr)) {
//             ptr++;
//         }

//         // add \0 to indicate end of the current arg, if string not finished yet
//         if (*ptr != '\0') {
//             *ptr = '\0';
//             ptr++; // find the next argument
//         }
//     }
    
//     result.args[result.argc] = NULL;

//     for (int i = 0; commands[i] != NULL; i++) {
//         if (strcmp(result.method, commands[i]) == 0) {
//             result.is_valid = 1;
//             printf("%s\n", commands[i]);
//             break;
//         }
//     }
    
//     // if command is not valid reset the command pointer to NULL for easy checking
//     if (!result.is_valid) {
//         result.method = NULL;
//     }

//     return result;
// }

int parse_commands(char *line, const char **valid_cmds, Command *results) {
    int command_count = 0;
    
    // splits the line by the '&' delimiter
    char *command_str = strtok(line, "&");

    while (command_str != NULL && command_count < MAX_COMMANDS) {
        Command *current_cmd = &results[command_count];
        current_cmd->method = NULL;
        current_cmd->argc = 0;
        current_cmd->is_valid = 0;

        char *ptr = command_str;

        while (*ptr && isspace((unsigned char)*ptr)) {
            ptr++;
        }

        if (*ptr == '\0') {
            command_str = strtok(NULL, "&");
            continue;
        }


        current_cmd->method = ptr;
        
        while (*ptr && !isspace((unsigned char)*ptr)) {
            ptr++;
        }
        
        if (*ptr) {
            *ptr = '\0';
            ptr++;
        }

        while (*ptr != '\0' && current_cmd->argc < MAX_ARGS - 1) {
            while (*ptr && isspace((unsigned char)*ptr)) {
                ptr++;
            }

            if (*ptr == '\0') {
                break;
            }

            current_cmd->args[current_cmd->argc] = ptr;
            current_cmd->argc++;

            while (*ptr && !isspace((unsigned char)*ptr)) {
                ptr++;
            }

            if (*ptr != '\0') {
                *ptr = '\0';
                ptr++;
            }
        }
        current_cmd->args[current_cmd->argc] = NULL;

        // validation of the parsing cmds
        for (int i = 0; valid_cmds[i] != NULL; i++) {
            if (strcmp(current_cmd->method, valid_cmds[i]) == 0) {
                current_cmd->is_valid = 1;
                break;
            }
        }
        
        if (!current_cmd->is_valid) {
            fprintf(stderr, "Error: command '%s' not found.\n", current_cmd->method);
            current_cmd->method = NULL;
        }

        command_count++;
        command_str = strtok(NULL, "&");
    }

    return command_count;
}



void background_task(Command cmd, char *DIR, char *BINARIES_DIR) {
    if (!cmd.is_valid) return;

    // id_t pid = fork();

    // if (pid<0) {
    //     perror("Running method in background failed");
    //     return;
    // }

    // if(pid == 0) {
    if(strcmp(cmd.method, "cat") == 0) {
        if (cmd.argc<1){
            printf("Usage: %s <filename>\n", cmd.method);
            return;
        }

        char PATH[1024];
        char FILE[1024];

        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/cat");
        strcpy(FILE, DIR);
        strcat(FILE, "/");
        strcat(FILE, cmd.args[0]);

        if (access(FILE, F_OK)==-1) {
            printf("File %s does not exist.\n", cmd.args[0]);
            return;
        }
        
        char *exec_args[3];
        exec_args[0] = "cat";
        exec_args[1] = FILE;
        exec_args[2] = NULL;

        pid_t pid = fork();
        if (pid < 0){
            printf("Failed to create child process.\n");
            exit(EXIT_FAILURE);
        }
        else if(pid == 0){
            execv(PATH, exec_args);
            perror("'cat' failed:");
            exit(EXIT_FAILURE);
        }
        else {
            wait(NULL);
        }
    }

    else if (strcmp(cmd.method, "touch") == 0) {
        if (cmd.argc<1){
            printf("Usage: %s <filename>\n", cmd.method);
            return;
        }

        char PATH[1024];
        char FILE[1024];
        
        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/touch");
        strcpy(FILE, DIR);
        strcat(FILE, "/");
        strcat(FILE, cmd.args[0]);

        char *exec_args[3];
        exec_args[0] = "touch";
        exec_args[1] = FILE;
        exec_args[2] = NULL;

        pid_t pid = fork();
        if (pid < 0){
            printf("Failed to create child process.\n");
        }
        else if(pid == 0){
            execv(PATH, exec_args);
            perror("'touch' failed:");

        }
        else {
            wait(NULL);
        }
    }

    else if (strcmp(cmd.method, "rm") == 0) {
        if (cmd.argc<1){
            printf("Usage: %s <filename>\n", cmd.method);
            return;
        }

        char PATH[1024];
        char FILE[1024];
        
        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/rm");
        strcpy(FILE, DIR);
        strcat(FILE, "/");
        strcat(FILE, cmd.args[0]);

        char *exec_args[3];
        exec_args[0] = "rm";
        exec_args[1] = FILE;
        exec_args[2] = NULL;

        pid_t pid = fork();
        if (pid < 0){
            printf("Failed to create child process.\n");

        }
        else if(pid == 0){
            execv(PATH, exec_args);
            perror("'rm' faile:");

        }
        else {
            wait(NULL);
        }
    }

    else if (strcmp(cmd.method, "cp") == 0) {
        if (cmd.argc!=2){
            printf("Usage: %s <source_file> <destination_file>\n", cmd.method);
            return;
        }

        char PATH[1024];
        char FILE_ORIGIN[1024];
        char FILE_DEST[1024];
        
        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/cp");
        strcpy(FILE_ORIGIN, DIR);
        strcat(FILE_ORIGIN, "/");
        strcat(FILE_ORIGIN, cmd.args[0]);
        strcpy(FILE_DEST, DIR);
        strcat(FILE_DEST, "/");
        strcat(FILE_DEST, cmd.args[1]);

        if (access(FILE_ORIGIN, F_OK)==-1) {
            printf("File %s does not exist.\n", cmd.args[0]);
            return;
        }

        char *exec_args[4];
        exec_args[0] = "cp";
        exec_args[1] = FILE_ORIGIN;
        exec_args[2] = FILE_DEST;
        exec_args[3] = NULL;

        pid_t pid = fork();
        if (pid < 0){
            printf("Failed to create child process.\n");

        }
        else if(pid == 0){
            execv(PATH, exec_args);
            perror("'cp' faile:");

        }
        else {
            wait(NULL);
        }
    }

    else if (strcmp(cmd.method, "grep") == 0) {
        if (cmd.argc!=3){
            printf("Usage: %s [-n] <string> <file>\n", cmd.method);
            return;
        }

        char PATH[1024];
        char FILE[1024];
        
        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/grep");
        strcpy(FILE, DIR);
        strcat(FILE, "/");
        strcat(FILE, cmd.args[2]);

        if (access(FILE, F_OK)==-1) {
            printf("File %s does not exist.\n", cmd.args[2]);                                        
            return;
        }
        
        char *exec_args[5];
        exec_args[0] = "grep";
        exec_args[1] = cmd.args[0];
        exec_args[2] = cmd.args[1];
        exec_args[3] = FILE;
        exec_args[4] = NULL;

        pid_t pid = fork();
        if (pid < 0){
            printf("Failed to create child process.\n");

        }
        else if(pid == 0){
            execv(PATH, exec_args);
            perror("'grep' faile:");

        }
        else {
            wait(NULL);
        }
    }

    else if (strcmp(cmd.method, "ls") == 0) {
        if (cmd.argc>2) {
            printf("Usage: %s [-l] [-a]\n", cmd.method);
            return;
        }

        char PATH[1024];
        char DIR_PATH[1024];
        
        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/ls");
        strcpy(DIR_PATH, DIR);
        
        int idx = 0;

        char *exec_args[6];
        exec_args[idx++] = "ls";

        for (int i=0; i<cmd.argc; i++) {
            exec_args[idx++] = cmd.args[i];
        }
        exec_args[idx++] = DIR_PATH;
        exec_args[idx] = NULL;


        pid_t pid = fork();
        if (pid < 0){
            printf("Failed to create child process.\n");

        }
        else if(pid == 0){
            execv(PATH, exec_args);
            perror("'grep' faile:");

        }
        else {
            wait(NULL);
        }
    }            

    else {
        printf("Command %s not recognized.\n", cmd.method);
    }
    return;
}

/*
! MAIN LOOP SECTION
*/
void mostrar_prompt(char *dir) {
    if (dir==NULL){
        printf("shellvis/> ");
    }
    else {
        printf("shellvis:%s> ", dir);
    }
    fflush(stdout);  
}


int cd_method(char *new_dir){
    return chdir(new_dir);
}

int pwd_method(char *dir){
    if (getcwd(dir, 1024)==NULL) return -1;
    return 0;
}

/* 
 * Função principal do shell
 * Os alunos devem implementar aqui o loop principal
 * A leitura de comandos, execução e outras funcionalidades podem ser implementadas em outras funções auxiliares e chamadas aqui
*/
void shellvis() {
    char linha[1024];
    
    char DIR[1024];
    char BINARIES_DIR[1024];
    
    pwd_method(DIR); // initialize DIR
    strcpy(BINARIES_DIR, DIR);
    strcat(BINARIES_DIR, "/bin");
    
    while (1) { 
        mostrar_prompt(DIR);
        if (fgets(linha, sizeof(linha), stdin) == NULL) {
            printf("\n");
            break; 
        }
        linha[strcspn(linha, "\n")] = 0;

        Command cmds[MAX_COMMANDS];
        int nCmds;

        nCmds = parse_commands(linha, commands, cmds);

        Command cmd;
        for (int i=0; i<nCmds; i++){
            cmd = cmds[i];
            if(cmd.method == NULL) {
                char *temp = linha;
                while(isspace((unsigned char)*temp)) temp++;
                if (*temp != '\0') {
                    printf("Error: Invalid command.\n");
                }
                continue;
            }

            printf("------------------------\n");
            printf("Parsing Result:\n");
            printf("  Command: '%s'\n", cmd.method);
            printf("  Arguments: %d\n", cmd.argc);
            for (int i=0; i<cmd.argc; i++) {
                printf("  Argument %d: %s\n", i, cmd.args[i]);
            }
            printf("  Is Valid: %s\n", cmd.is_valid ? "Yes" : "No");
            printf("------------------------\n");
            
            if (strcmp(cmd.method, "exit") == 0) {
                while (wait(NULL) > 0);
                goto exit_shellvis;
                break;
            }
            else if (strcmp(cmd.method, "pwd") == 0){
                pwd_method(DIR);
                printf("%s\n", DIR);
            }

            else if(strcmp(cmd.method, "cd") == 0) {
                if (cmd.argc<1){
                    printf("Usage: %s <filename>\n", cmd.method);

                }

                if (cd_method(cmd.args[0])==-1) {
                    printf("cd: %s: No such file or directory.\n", cmd.args[0]);

                }
                pwd_method(DIR);
            }
            else {
                background_task(cmd, DIR, BINARIES_DIR);
            }
        }

        // wait all child processes running in the background
        while (wait(NULL) > 0);
    }

exit_shellvis:
    // exits from all loops when exit is called
    return;
}

int main() {
    // chamada obrigatória
    shellvis();    
    return 0;
}
