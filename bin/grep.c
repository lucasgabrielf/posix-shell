#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h> // used only for atoi method

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    /*
    searches for <string> in the specified file, and prints at most n
    lines where the <string> is found
    */

    if (argc != 4 || strcmp(argv[3], "") == 0) {
        printf("Usage: %s [-n] <string> <file>\n", argv[0]);
        return -1;
    }

    int n = atoi(argv[1]);
    
    if (n<=0) {
        printf("Input a valid number of lines.\n");
        return 0;
    }

    int fd = open(argv[3], O_RDONLY);

    if (fd == -1) {
        perror("Error opening file:");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    char line_buffer[BUFFER_SIZE];
    int line_pos = 0;
    int lines_found = 0;

    // read the file in chunks then process it character by character
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            // if the character is not a newline add it to line buffer
            if (buffer[i] != '\n') {
                if (line_pos < BUFFER_SIZE - 1) {
                    line_buffer[line_pos++] = buffer[i];
                }
            } else {
                line_buffer[line_pos] = '\0';

                if (strstr(line_buffer, argv[2]) != NULL) {
                    printf("%s\n", line_buffer);
                    lines_found++;
                    if (lines_found >= n) {
                        goto end_loop;
                    }
                }
                line_pos = 0;
            }
        }
    }

end_loop:;

    // check if there si a final line without a newline
    if (line_pos > 0 && lines_found < n) {
        line_buffer[line_pos] = '\0';
        if (strstr(line_buffer, argv[2]) != NULL) {
            printf("%s\n", line_buffer);
        }
    }


    if (close(fd) == -1) {
        perror("Error closing file:");
        return -1;
    }

    return 0;
}