#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#define MAX_PATH_SIZE 1024
// TODO: https://stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c

#define USAGE  \
    "usage:\n" \
    "    mls [options] [path]\n"\
    "options:\n" \
    "    -1          output has one entry per line\n"\
    "    -a          include hidden files (i.e. that begin with a dot)\n"\
    "    -p          if an entry is a directory, add a \"/\" at the end\n"\
    "    -h          prints out this message\n"\
    "path:\n" \
    "    A path to a file or directory. If none is provided, defaults to current directory.\n"\


void my_ls(char *file_name);
void walk_directory(char *directory_name, void (*function_for_files)(char *function_arg));

int main(int argc, char **argv)
{

    // PARSE FLAGS
    char option_char;
    int new_lines, include_hidden_files, add_dir_slashes = 0;

    // CITATION: K&R 5.10, p. 117
    while (--argc > 0 && (*++argv)[0] == '-') {
        while ((option_char = *++argv[0])) {
            fprintf(stdout, "Processing option %c\n", option_char);
            
            switch (option_char) {
                case '1':
                    new_lines = 1;
                    break;
                case 'a':
                    include_hidden_files = 1;
                    break;
                case 'p':
                    add_dir_slashes = 1;
                    break;
                case 'h':
                    argc = 0;
                    fprintf(stdout, "%s", USAGE);
                    exit(0);
                    break;
                default:
                    argc = 0;
                    fprintf(stderr, "Cannot handle option: %c\n", option_char);
                    fprintf(stderr, "%s", USAGE);
                    exit(1);
                    break;
            }
        }
    }
    
    // CALL INTO LS
    if (argc == 0) {
        my_ls(".");
    } else {
        my_ls(*argv);
    }

    // DONE
    exit(0);
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
