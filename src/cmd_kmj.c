#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "mycmd.h"

int my_ls(int argc, char **argv)
{
    DIR *dp;
    struct dirent *d;
    char *path = ".";

    if (argc > 1 && argv[1][0] != '-')
        path = argv[1];
    if ((dp = opendir(path)) == NULL) {
        perror("ls");
        return 1;
    }
    while ((d = readdir(dp)) != NULL) {
        if (d->d_name[0] == '.')
            continue;
        printf("%s  ", d->d_name);
    }
    printf("\n");
    closedir(dp);
    return 0;
}

int my_pwd(int argc, char **argv)
{
    char buf[256];

    if (getcwd(buf, 256) == NULL) {
        perror("pwd");
        return 1;
    }
    printf("%s\n", buf);
    return 0;
}

int my_cd(int argc, char **argv)
{
    char *path;

    if (argc < 2)
        path = getenv("HOME");
    else
        path = argv[1];
    if (chdir(path) == -1) {
        perror("cd");
        return 1;
    }
    return 0;
}

int my_mkdir(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: mkdir dir\n");
        return 1;
    }
    if (mkdir(argv[1], 0755) == -1) {
        perror("mkdir");
        return 1;
    }
    return 0;
}
