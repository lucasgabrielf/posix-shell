#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]) {
    /*
    deletes a file
    */

    // argv[1] = complete absolute path for filename to be deleted
    if (argc != 2 || strcmp(argv[1], "") == 0) {
        printf("Usage: %s <filename>\n", argv[0]);
        return -1;
    }

    if (unlink(argv[1]) == -1) {
        perror("Could not delete file:");
        return -1;
    }

    return 0;
}
