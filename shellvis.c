#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 16 // maximum number of arguments passed

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

Command parse_command(char *line, const char **commands){
    // parser created to populate the scruct with the correct method and arguments list
    Command result = {NULL, NULL, 0};
    result.argc = 0;
    char *ptr = line;

    while (*ptr && isspace((unsigned char)*ptr)) {
        ptr++;
    }
    
    if (*ptr == '\0') {
        return result;
    }

    result.method = ptr;

    while (*ptr && !isspace((unsigned char)*ptr)) {
        ptr++;
    }

    *ptr = '\0';
    ptr++;

    while (*ptr != '\0' && result.argc < MAX_ARGS - 1) {
        // skp white spaces and if null terminaded, finish
        while (*ptr && isspace((unsigned char)*ptr)) {
            ptr++;
        }

        if (*ptr == '\0') {
            break;
        }

        // saves the pointer to the start of new arg
        result.args[result.argc] = ptr;
        result.argc++;

        while (*ptr && !isspace((unsigned char)*ptr)) {
            ptr++;
        }

        // add \0 to indicate end of the current arg, if string not finished yet
        if (*ptr != '\0') {
            *ptr = '\0';
            ptr++; // find the next argument
        }
    }
    
    result.args[result.argc] = NULL;

    for (int i = 0; commands[i] != NULL; i++) {
        if (strcmp(result.method, commands[i]) == 0) {
            result.is_valid = 1;
            printf("%s\n", commands[i]);
            break;
        }
    }
    
    // if command is not valid reset the command pointer to NULL for easy checking
    if (!result.is_valid) {
        result.method = NULL;
    }

    return result;
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

        Command cmd = parse_command(linha, commands);

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
        
        if (strcmp(cmd.method, "exit") == 0)
            break;

        else if (strcmp(cmd.method, "pwd") == 0){
            pwd_method(DIR);
            printf("%s\n", DIR);
            continue;
        }

        else if(strcmp(cmd.method, "cd") == 0) {
            if (cmd.argc<1){
                printf("Usage: %s <filename>\n", cmd.method);
                continue;
            }

            if (cd_method(cmd.args[0])==-1) {
                printf("cd: %s: No such file or directory.\n", cmd.args[0]);
                continue;
            }
            pwd_method(DIR);
            continue;
        }

        else if(strcmp(cmd.method, "cat") == 0) {
            if (cmd.argc<1){
                printf("Usage: %s <filename>\n", cmd.method);
                continue;
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
                continue;
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
                continue;
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
                continue;
            }
            else if(pid == 0){
                execv(PATH, exec_args);
                perror("'touch' failed:");
                continue;
            }
            else {
                wait(NULL);
            }
        }

        else if (strcmp(cmd.method, "rm") == 0) {
            if (cmd.argc<1){
                printf("Usage: %s <filename>\n", cmd.method);
                continue;
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
                continue;
            }
            else if(pid == 0){
                execv(PATH, exec_args);
                perror("'rm' faile:");
                continue;
            }
            else {
                wait(NULL);
            }
        }

        else if (strcmp(cmd.method, "cp") == 0) {
            if (cmd.argc!=2){
                printf("Usage: %s <source_file> <destination_file>\n", cmd.method);
                continue;
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

            char *exec_args[4];
            exec_args[0] = "cp";
            exec_args[1] = FILE_ORIGIN;
            exec_args[2] = FILE_DEST;
            exec_args[3] = NULL;

            pid_t pid = fork();
            if (pid < 0){
                printf("Failed to create child process.\n");
                continue;
            }
            else if(pid == 0){
                execv(PATH, exec_args);
                perror("'cp' faile:");
                continue;
            }
            else {
                wait(NULL);
            }
        }

        else if (strcmp(cmd.method, "grep") == 0) {
            if (cmd.argc!=3){
                printf("Usage: %s [-n] <string> <file>\n", cmd.method);
                continue;
            }

            char PATH[1024];
            char FILE[1024];
            
            strcpy(PATH, BINARIES_DIR);
            strcat(PATH, "/grep");
            strcpy(FILE, DIR);
            strcat(FILE, "/");
            strcat(FILE, cmd.args[2]);
            
            char *exec_args[4];
            exec_args[0] = "grep";
            exec_args[1] = cmd.args[0];
            exec_args[2] = cmd.args[1];
            exec_args[3] = FILE;
            exec_args[4] = NULL;

            pid_t pid = fork();
            if (pid < 0){
                printf("Failed to create child process.\n");
                continue;
            }
            else if(pid == 0){
                execv(PATH, exec_args);
                perror("'grep' faile:");
                continue;
            }
            else {
                wait(NULL);
            }
        }
        
        else {
            printf("Command %s not recognized.\n", cmd.method);
        }
    }
}

int main() {
    // chamada obrigatória
    shellvis();    
    return 0;
}
