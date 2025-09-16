#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>  
#include <sys/stat.h>
#include <unistd.h>  
#include <pwd.h>     
#include <grp.h>     
#include <time.h>    

void print_long_format(const char *path, const char *name);

int main(int argc, char *argv[]) {
    /*
    implementation of the ls method, accepting both -l and -a flags (as weel as its combinations)
    */
    const char *dir_path = "."; 
    int long_format = 0;        // flag to check if -l is true
    int show_hidden = 0;        // flag to check if -a is true

    // this parser handles flags like -l, -a, and combined flags like -la.
    for (int i = 0; i < argc; i++) {
        // starts with '-'?
        if (argv[i][0] == '-') {
            for (int j = 1; argv[i][j] != '\0'; j++) {
                switch(argv[i][j]) {
                    case 'l':
                        long_format = 1;
                        break;
                        case 'a':
                        show_hidden = 1;
                        break;
                    default:
                        printf("%s: invalid option -- '%c'\n", argv[0], argv[i][j]);
                        printf("Usage: %s [-la] [path]\n", argv[0]);
                        exit(EXIT_FAILURE);
                }
            }
        } else {
            // if it doesn't start with '-' assume it's the directory path
            dir_path = argv[i];
        }
    }

    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir failed");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;

    // read each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // skip hidden filess unless -a is specified
        if (!show_hidden && entry->d_name[0] == '.') {
            continue;
        }

        if (long_format) {
            print_long_format(dir_path, entry->d_name);
        } else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
    return 0;
}


void print_long_format(const char *path, const char *name) {
    // prints file information in a format similar to 'ls -l'
    // construct the full path to the file for lstat
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", path, name);

    struct stat file_stat;
    // use lstat to get info about the file itself, not what it points to (if it's a symlink)
    if (lstat(full_path, &file_stat) == -1) {
        perror("lstat failed");
        return;
    }

    // permissions
    // file type
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((S_ISLNK(file_stat.st_mode)) ? "l" : "-");
    // user permissions
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    // group permissions
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    // other permissions
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

    // number of hard links
    printf(" %2lu", file_stat.st_nlink);

    // owner name
    struct passwd *pw = getpwuid(file_stat.st_uid);
    if (pw != NULL) {
        printf(" %-8s", pw->pw_name);
    } else {
        printf(" %-8d", file_stat.st_uid);
    }

    // group name
    struct group *gr = getgrgid(file_stat.st_gid);
    if (gr != NULL) {
        printf(" %-8s", gr->gr_name);
    } else {
        printf(" %-8d", file_stat.st_gid);
    }

    // file size
    printf(" %8lld", (long long)file_stat.st_size);

    // modification time
    char time_buf[80];
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    // debian format: "Sep 08 15:46"
    strftime(time_buf, sizeof(time_buf), "%b %d %H:%M", timeinfo);
    printf(" %s", time_buf);
    
    printf(" %s\n", name);
}

