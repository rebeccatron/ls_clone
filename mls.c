#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#define MAX_PATH_SIZE 1024
// TODO: https://stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c

void my_ls(char *file_name);
void walk_directory(char *directory_name, void (*function_for_files)(char *function_arg));

int main(int argc, char **argv)
{
    if (argc == 1)
        my_ls(".");
    else
        while (--argc > 0)
            my_ls(*++argv);

    return 0;
}

void my_ls(char *file_name)
{
    struct stat file_stats;

    if (stat(file_name, &file_stats) == -1) {
        fprintf(stderr, "mls:%d\tCannot access file/directory: %s.\n", __LINE__, file_name);
        return;
    }

    if ((file_stats.st_mode & S_IFMT) == S_IFDIR)
        walk_directory(file_name, my_ls);

    printf("%8lld Bytes %s\n", file_stats.st_size, file_name);
}

void walk_directory(char *directory_name, void (*function_for_files)(char *function_arg))
{
    DIR *directory;
    struct dirent *file;
    char name[MAX_PATH_SIZE];

    // open the directory
    if ((directory = opendir(directory_name)) == NULL) {
        fprintf(stderr, "mls:%d\tCannot access file/directory: %s.\n", __LINE__, directory_name);
        return;
    }

    // loop through the files in the directory
    while ((file = readdir(directory)) != NULL) {
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0)
            continue; // skip current directory + parent

        if (strlen(directory_name) + strlen(file->d_name) + 2 > sizeof(name)) {
            fprintf(stderr, "mls:%d\tFile name too long to print! %s/%s.\n", __LINE__, directory_name, file->d_name);
        } else {
            sprintf(name, "%s/%s", directory_name, file->d_name);
            (*function_for_files)(name);
        }
    }

    closedir(directory);
}
