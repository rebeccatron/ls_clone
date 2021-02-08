#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "dirent.h"

#define MAX_PATH_SIZE 1024
// TODO: https://stackoverflow.com/questions/13554150/implementing-the-ls-al-command-in-c

// CITATION: https://timseverien.github.io/binary-cheatsheet/
#define F_NEW_LINES 1
#define F_ALL_FILES 2
#define F_DIR_SLASH 4
#define F_FILE_SIZE 8

#define USAGE  \
    "usage:\n" \
    "    mls [options] [path]\n"\
    "options:\n" \
    "    -1          output has one entry per line\n"\
    "    -a          include hidden files (i.e. that begin with a dot)\n"\
    "    -p          if an entry is a directory, add a \"/\" at the end\n"\
    "    -s          show file size (in bytes)\n"\
    "    -h          prints out this message\n"\
    "path:\n" \
    "    A path to a file or directory. If none is provided, defaults to current directory.\n"\


void my_ls(char *path, int flag);

int main(int argc, char **argv)
{

    // PARSE FLAGS
    char option_char;
    unsigned int flag = 0;

    // CITATION: K&R 5.10, p. 117
    while (--argc > 0 && (*++argv)[0] == '-') {
        while ((option_char = *++argv[0])) {
            switch (option_char) {
                case '1':
                    flag |= F_NEW_LINES;
                    break;
                case 'a':
                    flag |= F_ALL_FILES;
                    break;
                case 'p':
                    flag |= F_DIR_SLASH;
                    break;
                case 's':
                    flag |= F_FILE_SIZE;
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
        my_ls(".", flag);
    } else {
        my_ls(*argv, flag);
    }

    // DONE
    exit(0);
}

void print(char *path, struct stat *file_stats, int flag)
{
    if (flag & F_FILE_SIZE)
        printf("%8lld Bytes  ", file_stats->st_size);

    char* postfix;
    
    if ((flag & F_DIR_SLASH) && ((file_stats->st_mode & S_IFMT) == S_IFDIR)) {
        postfix = "/";
    } else {
        postfix = "";
    }

    printf("%s%s\t", path, postfix);
    
    if (flag & F_NEW_LINES)
        printf("\n");
}

void walk_directory(char *directory_name, int flag)
{
    DIR *directory;
    struct dirent *file;

    // open the directory
    if ((directory = opendir(directory_name)) == NULL) {
        fprintf(stderr, "mls:%d\tCannot access directory: %s.\n", __LINE__, directory_name);
        exit(1);
    }

    // loop through the files in the directory
    while ((file = readdir(directory)) != NULL) {
        if (file->d_name[0] == '.' && !(flag & F_ALL_FILES)) {
            continue;
        }

        struct stat file_stats;
        if (stat(file->d_name, &file_stats) == -1) {
            fprintf(stderr, "mls:%d\tCannot access file/directory: %s.\n", __LINE__, file->d_name);
            closedir(directory);
            exit(1);
        }

        print(file->d_name, &file_stats, flag);
    }

    closedir(directory);
}

void my_ls(char *path, int flag)
{
    struct stat file_stats;

    if (stat(path, &file_stats) == -1) {
        fprintf(stderr, "mls:%d\tCannot access path: %s.\n", __LINE__, path);
        exit(1);
    }

    if ((file_stats.st_mode & S_IFMT) == S_IFDIR) {
        walk_directory(path, flag);
    } else {
        print(path, &file_stats, flag);
    }
}
