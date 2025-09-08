#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> // for O_CREAT | O_RDWR | O_TRUNC
#include <string.h>

int main(int argc, char *argv[]) {
    /*
    creates a new file in the specified path
    */

    // argv[1] = complete absolute path for filename to be created
    if (argc != 2 || strcmp(argv[1], "") == 0) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    // 0644: File permissions (owner can read/write, group/others can read)
    // mode is REQUIRED when using O_CREAT
    int fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);

    if (fd == -1) {
        perror("Error creating file:");
        return -1;
    }

    if (close(fd) == -1) {
        perror("Error closing file:");
        return -1;
    }

    return 0;
}
