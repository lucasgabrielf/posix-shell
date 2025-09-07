#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *DIR_FILE = argv[1];
    if (access(DIR_FILE, F_OK)==-1) {
        printf("File %s does not exist.\n");
        return -1;
    printf("okokok\n");
    return 0;
}