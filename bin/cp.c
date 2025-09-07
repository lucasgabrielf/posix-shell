#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h> // for stat

// Define a buffer size for reading chunks of the file. 4KB is a common size.
#define BUFFER_SIZE 4096

int is_dir(char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        // error getting status
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

int main(int argc, char *argv[]) {
    /*
    copies the content of a file <origin> into the file <destination>.
    if <destination> file name is a directory, its name will be the same of the <origin>
    file, but in the specified directory
    
    it will attempt to overwrite or create a file with that name
    */

    if (argc != 3) {
        printf("Usage: %s <source_file> <destination_file>\n", argv[0]);
        return -1;
    }

    int fd_origin = open(argv[1], O_RDONLY);
    if (fd_origin == -1) {
        perror("Error opening origin file");
        return -1;
    }

    // O_TRUNC: if the file already exists, truncate it to zero length
    // 0644: File permissions (owner can read/write, group/others can read)
    // mode is REQUIRED when using O_CREAT
    int fd_dest = open(argv[2], O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd_dest == -1) {
        perror("Error opening/creating destination file");
        close(fd_origin);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd_origin, buffer, BUFFER_SIZE)) > 0) {
        ssize_t bytes_written = write(fd_dest, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to destination file");
            close(fd_origin);
            close(fd_dest);
            return -1;
        }
    }

    if (bytes_read == -1) {
        perror("Error reading from origin file");
        close(fd_origin);
        close(fd_dest);
        return -1;
    }

    if (close(fd_origin) == -1) {
        perror("Error closing origin file");
        return -1;
    }

    if (close(fd_dest) == -1) {
        perror("Error closing destination file");
        return -1;
    }

    return 0;
}