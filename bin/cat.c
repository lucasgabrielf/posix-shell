#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h> // O_RDONLY

#define BUFFER 4096
int main(int argc, char *argv[]) {
    /*
    this implementation of cat loads a chunk of data from the file into the buffer,
    reads it up to the newline character '\n' or EOF character, and prints it, then moves the
    buffer to this point and loads into memory again in a loop
    */

    // argv[1] = complete absolute path for filename to read from
    if (argc != 2 || strcmp(argv[1], "") == 0) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    // openning file with read only mode
    int fd = open(argv[1], O_RDONLY);

    if (fd==-1) {
        perror("Error opening file:");
        return -1;
    }

    char buffer[BUFFER];
    ssize_t number_bytes;
    int file_pointer = 0; // points to how many bytes in the buffer is part of an imcomplete line
    // so the buffer dont overwrite it

    while(1) {
        number_bytes = read(fd, buffer+file_pointer, BUFFER-file_pointer); //

        if (number_bytes == -1) {
            perror("Error reading file:");
            close(fd);
            return -1;
        }

        // total bytes we now have in the buffer to process
        ssize_t bytes_to_process = file_pointer + number_bytes;
        
        // if we read 0 bytes and have no leftovers, we are done
        int eof_reached = (file_pointer == 0);
        
        if (bytes_to_process == 0) {
            break;
        }

        char *line_start = buffer;
        char *current_pos = buffer;

        // scan the buffer for newlines
        for (int i = 0; i < bytes_to_process; i++) {
            if (buffer[i] == '\n') {
                int line_length = &buffer[i] - line_start + 1;
                write(STDOUT_FILENO, line_start, line_length);
                line_start = &buffer[i] + 1; // Start of the next line
            }
        }
        
        // after the loop, `line_start` points to the beginning of the
        // next incomplete line, so we need to move this data to the
        // start of the buffer for the next read cycle and update the 
        // file_pointer
        file_pointer = bytes_to_process - (line_start - buffer);
        memmove(buffer, line_start, file_pointer);

        if (eof_reached) {
            break;
        }
    }
        
    // after the loop, there might be one last line without a trailing newline
    // if there's anything left in our file_pointer, print it
    if (file_pointer > 0) {
        write(STDOUT_FILENO, buffer, file_pointer);
    }

    close(fd);
    printf("\n");

    return 0;
}