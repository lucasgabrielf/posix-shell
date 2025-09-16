/**
 *                              ████                                                 █████                         
 *                             ▒▒███                                                ▒▒███                          
 *     █████ ███ █████  ██████  ▒███   ██████   ██████  █████████████    ██████     ███████    ██████              
 *    ▒▒███ ▒███▒▒███  ███▒▒███ ▒███  ███▒▒███ ███▒▒███▒▒███▒▒███▒▒███  ███▒▒███   ▒▒▒███▒    ███▒▒███             
 *     ▒███ ▒███ ▒███ ▒███████  ▒███ ▒███ ▒▒▒ ▒███ ▒███ ▒███ ▒███ ▒███ ▒███████      ▒███    ▒███ ▒███             
 *     ▒▒███████████  ▒███▒▒▒   ▒███ ▒███  ███▒███ ▒███ ▒███ ▒███ ▒███ ▒███▒▒▒       ▒███ ███▒███ ▒███             
 *      ▒▒████▒████   ▒▒██████  █████▒▒██████ ▒▒██████  █████▒███ █████▒▒██████      ▒▒█████ ▒▒██████              
 *       ▒▒▒▒ ▒▒▒▒     ▒▒▒▒▒▒  ▒▒▒▒▒  ▒▒▒▒▒▒   ▒▒▒▒▒▒  ▒▒▒▒▒ ▒▒▒ ▒▒▒▒▒  ▒▒▒▒▒▒        ▒▒▒▒▒   ▒▒▒▒▒▒               
 *                                                                                                                 
 *                                                                                                                 
 *                                                                                                                 
 *     ███████████                    ███                                █████████  █████               ████  ████ 
 *    ▒▒███▒▒▒▒▒███                  ▒▒▒                                ███▒▒▒▒▒███▒▒███               ▒▒███ ▒▒███ 
 *     ▒███    ▒███  ██████   █████  ████  █████ █████                 ▒███    ▒▒▒  ▒███████    ██████  ▒███  ▒███ 
 *     ▒██████████  ███▒▒███ ███▒▒  ▒▒███ ▒▒███ ▒▒███     ██████████   ▒▒█████████  ▒███▒▒███  ███▒▒███ ▒███  ▒███ 
 *     ▒███▒▒▒▒▒▒  ▒███ ▒███▒▒█████  ▒███  ▒▒▒█████▒     ▒▒▒▒▒▒▒▒▒▒     ▒▒▒▒▒▒▒▒███ ▒███ ▒███ ▒███████  ▒███  ▒███ 
 *     ▒███        ▒███ ▒███ ▒▒▒▒███ ▒███   ███▒▒▒███                   ███    ▒███ ▒███ ▒███ ▒███▒▒▒   ▒███  ▒███ 
 *     █████       ▒▒██████  ██████  █████ █████ █████                 ▒▒█████████  ████ █████▒▒██████  █████ █████
 *    ▒▒▒▒▒         ▒▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒▒ ▒▒▒▒▒                   ▒▒▒▒▒▒▒▒▒  ▒▒▒▒ ▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒▒ ▒▒▒▒▒ 
 *                                                                                                                 
 *                                                                                                                 
 *                                                                                                                 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_ARGS 16 // maximum number of arguments passed
#define MAX_COMMANDS 8

const char *commands[] = {
    "pwd",
    "cd",
    "path",
    "exit",
    "cat",
    "touch",
    "rm",
    "ls",
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

//* list of methods created for the execution of the program
// parse the terminal line the user wrote; also, separates the commands between '&'
int parse_commands(char *line, const char **valid_cmds, Command *results);

// execute a command in a child process
void execute_command(char *executable_path, Command *cmd);

// searches in path and executes a program binary in a child process
void execute_program(const Command *cmd);

// method that orchestrates the execution of the cmd commands with execute_command
// and execute_program
void background_task(Command cmd, char *DIR, char *BINARIES_DIR);

// util methods
int cd_method(char *new_dir);
int pwd_method(char *dir);
int isspace(int c);

void mostrar_prompt(char *dir);
void posix_shell();
void wellcome(char *DIR, char *BINARIES_DIR);

int main() {
    // chamada obrigatória
    posix_shell();
    return 0;
}

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
            if (strstr(current_cmd->method, "./") != NULL) {
                // method is a program
                current_cmd->is_valid = 1;
            }
            else{
                fprintf(stderr, "Error: command '%s' not found.\n", current_cmd->method);
                current_cmd->method = NULL;
            }
        }

        command_count++;
        command_str = strtok(NULL, "&");
    }

    return command_count;
}

void execute_command(char *executable_path, Command *cmd) {
    char *inputFile = NULL;
    char *outputFile = NULL;
    char *clean_args[MAX_ARGS + 1]; // +1 to ensure space for the command name
    int clean_argc = 0;

    clean_args[clean_argc++] = cmd->method;

    for (int i = 0; i < cmd->argc; i++) {
        if (strcmp(cmd->args[i], "<") == 0) {
            if (++i < cmd->argc) {
                inputFile = cmd->args[i];
            } else {
                fprintf(stderr, "Syntax error: no file for input redirection.\n");
                return;
            }
        } else if (strcmp(cmd->args[i], ">") == 0) {
            if (++i < cmd->argc) {
                outputFile = cmd->args[i];
            } else {
                fprintf(stderr, "Syntax error: no file for output redirection.\n");
                return;
            }
        } else {
            if (clean_argc < MAX_ARGS) {
                clean_args[clean_argc++] = cmd->args[i];
            } else {
                fprintf(stderr, "Error: too many arguments.\n");
                return;
            }
        }
    }
    clean_args[clean_argc] = NULL;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        return;
    }
    else if (pid == 0) {
        // input redirection
        if (inputFile) {
            int input_fd = open(inputFile, O_RDONLY);
            if (input_fd == -1) {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            if (dup2(input_fd, STDIN_FILENO) == -1) {
                perror("dup2 failed for stdin");
                exit(EXIT_FAILURE);
            }
            close(input_fd);
        }

        // output redirection
        if (outputFile) {
            int output_fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd == -1) {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            if (dup2(output_fd, STDOUT_FILENO) == -1) {
                perror("dup2 failed for stdout");
                exit(EXIT_FAILURE);
            }
            close(output_fd);
        }

        execv(executable_path, clean_args);

        perror("execv failed");
        exit(EXIT_FAILURE);
    }
    else {
        // this line makes the execution of commands between & sequential instead of parallel
        // wait(NULL);
    }
}

void execute_program(const Command *cmd) {
    if (cmd == NULL || cmd->method == NULL) {
        fprintf(stderr, "Error: Invalid command provided.\n");
        return;
    }

    char *inputFile = NULL;
    char *outputFile = NULL;
    
    char *clean_args[MAX_ARGS + 2];
    int clean_argc = 0;

    char program_name[1024];
    strncpy(program_name, cmd->method+2, (int)strlen(cmd->method));
    
    clean_args[clean_argc++] = program_name;

    for (int i = 0; i < cmd->argc; i++) {
        // Input Redirection
        if (strcmp(cmd->args[i], "<") == 0) {
            if (++i < cmd->argc) {
                inputFile = cmd->args[i];
            } else {
                fprintf(stderr, "Syntax error: No file specified for input redirection.\n");
                return;
            }
        } 
        else if (strcmp(cmd->args[i], ">") == 0) {
            if (++i < cmd->argc) {
                outputFile = cmd->args[i];
            } else {
                fprintf(stderr, "Syntax error: No file specified for output redirection.\n");
                return;
            }
        } 
        else {
            if (clean_argc < (MAX_ARGS + 1)) {
                clean_args[clean_argc++] = cmd->args[i];
            } else {
                fprintf(stderr, "Error: Too many arguments.\n");
                return;
            }
        }
    }
    clean_args[clean_argc] = NULL;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    } 
    else if (pid == 0) {
        if (inputFile) {
            int input_fd = open(inputFile, O_RDONLY);
            if (input_fd == -1) {
                perror("Failed to open input file");
                exit(EXIT_FAILURE);
            }
            if (dup2(input_fd, STDIN_FILENO) == -1) {
                perror("dup2 failed for stdin");
                exit(EXIT_FAILURE);
            }
            close(input_fd);
        }

        if (outputFile) {
            int output_fd = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd == -1) {
                perror("Failed to open output file");
                exit(EXIT_FAILURE);
            }
            if (dup2(output_fd, STDOUT_FILENO) == -1) {
                perror("dup2 failed for stdout");
                exit(EXIT_FAILURE);
            }
            close(output_fd);
        }
        
        execvp(clean_args[0], clean_args);

        fprintf(stderr, "Error: command not found: %s\n", clean_args[0]);
        exit(EXIT_FAILURE);
    } 
    else {
        // this line makes the execution of commands between & sequential instead of parallel
        // wait(NULL);
    }
}

void background_task(Command cmd, char *DIR, char *BINARIES_DIR) {
    if (!cmd.is_valid) return;

    if(strcmp(cmd.method, "cat") == 0) {
        if (cmd.argc < 1){
            printf("Usage: %s <filename>\n", cmd.method);
            return;
        }

        char PATH[1024];
        char FILE[1024];

        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/cat"); // exe path
        strcpy(FILE, DIR);
        strcat(FILE, "/");
        strcat(FILE, cmd.args[0]);
        cmd.args[0] = FILE;

        if (access(FILE, F_OK)==-1) {
            printf("File %s does not exist.\n", cmd.args[0]);
            return;
        }

        execute_command(PATH, &cmd);
    }

    else if (strcmp(cmd.method, "touch") == 0) {
        if (cmd.argc < 1){
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
        cmd.args[0] = FILE;

        execute_command(PATH, &cmd);

    }

    else if (strcmp(cmd.method, "rm") == 0) {
        if (cmd.argc < 1){
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
        cmd.args[0] = FILE;
        execute_command(PATH, &cmd);
    }

    else if (strcmp(cmd.method, "cp") == 0) {
        if (cmd.argc < 2){
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
        cmd.args[0] = FILE_ORIGIN;

        strcpy(FILE_DEST, DIR);
        strcat(FILE_DEST, "/");
        strcat(FILE_DEST, cmd.args[1]);
        cmd.args[1] = FILE_DEST;


        if (access(FILE_ORIGIN, F_OK)==-1) {
            printf("File %s does not exist.\n", cmd.args[0]);
            return;
        }
        execute_command(PATH, &cmd);
    }

    else if (strcmp(cmd.method, "grep") == 0) {
        if (cmd.argc < 3){
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
        cmd.args[2] = FILE;

        if (access(FILE, F_OK)==-1) {
            printf("File %s does not exist.\n", cmd.args[2]);
            return;
        }
        execute_command(PATH, &cmd);
    }

    else if (strcmp(cmd.method, "ls") == 0) {
        char PATH[1024];
        char DIR_PATH[1024];

        strcpy(PATH, BINARIES_DIR);
        strcat(PATH, "/ls");
        strcpy(DIR_PATH, DIR);

        int idx = 0;

        char *exec_args[6];
        exec_args[idx++] = "ls";

        cmd.args[cmd.argc++] = DIR_PATH;
        cmd.args[cmd.argc] = NULL;

        execute_command(PATH, &cmd);
    }

    else if (strstr(cmd.method, "./") != NULL) {
        execute_program(&cmd);
    }

    else {
        printf("Command %s not recognized.\n", cmd.method);
    }
}

/*
! MAIN LOOP SECTION
*/
void mostrar_prompt(char *dir) {
    if (dir==NULL){
        printf("Posix-Shell/> ");
    }
    else {
        printf("Posix-Shell:%s> ", dir);
    }
    // printf("Posix-Shell> ");
    fflush(stdout);
}


int cd_method(char *new_dir){
    return chdir(new_dir);
}

int pwd_method(char *dir){
    if (getcwd(dir, 1024)==NULL) return -1;
    return 0;
}

void wellcome(char *DIR, char *BINARIES_DIR){
    char linha[] = "cat greetings.txt";
    linha[strcspn(linha, "\n")] = 0;
    Command cmds[MAX_COMMANDS];
    parse_commands(linha, commands, cmds);
    Command cmd = cmds[0];
    background_task(cmd, DIR, BINARIES_DIR);
    while(wait(NULL)>1);
    return;
}

/*
 * Função principal do shell
 * Os alunos devem implementar aqui o loop principal
 * A leitura de comandos, execução e outras funcionalidades podem ser implementadas em outras funções auxiliares e chamadas aqui
*/
void posix_shell() {
    char linha[1024];

    char DIR[1024];
    char BINARIES_DIR[1024];

    pwd_method(DIR); // initialize DIR
    strcpy(BINARIES_DIR, DIR);
    strcat(BINARIES_DIR, "/bin");

    wellcome(DIR, BINARIES_DIR);

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

            // printf("------------------------\n");
            // printf("parsing Result:\n");
            // printf("  command: '%s'\n", cmd.method);
            // printf("  arguments: %d\n", cmd.argc);
            // for (int i=0; i<cmd.argc; i++) {
            //     printf("  argument %d: %s\n", i, cmd.args[i]);
            // }
            // printf("  is Valid: %s\n", cmd.is_valid ? "Yes" : "No");
            // printf("------------------------\n");

            if (strcmp(cmd.method, "exit") == 0) {
                while (wait(NULL) > 0);
                goto exit_posix_shell;
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

            else if (strcmp(cmd.method, "path") == 0) {
                if (cmd.argc == 0) {
                    char *current_path = getenv("PATH");
                    if (current_path) {
                        printf("%s\n", current_path);
                    }
                } else {
                    size_t total_len = 0;
                    for (int i = 0; i < cmd.argc; i++) {
                        total_len += strlen(cmd.args[i]);
                    }
                    total_len += cmd.argc; 

                    char *new_path = malloc(total_len);
                    if (new_path == NULL) {
                        perror("malloc");
                        continue;
                    }

                    char *p = new_path;
                    for (int i = 0; i < cmd.argc; i++) {
                        size_t arg_len = strlen(cmd.args[i]);
                        memcpy(p, cmd.args[i], arg_len);
                        p += arg_len;
                        if (i < cmd.argc - 1) {
                            *p = ':';
                            p++;
                        }
                    }
                    *p = '\0';

                    if (setenv("PATH", new_path, 1) == -1) {
                        perror("setenv");
                        free(new_path); 
                    }
                }
            }

            else {
                background_task(cmd, DIR, BINARIES_DIR);
            }
        }

        // wait all child processes running in the background
        while (wait(NULL) > 0);
    }

exit_posix_shell:
    // exits from all loops when exit is called
    return;
}
