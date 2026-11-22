#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mycmd.h"

int my_mv(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: mv old new\n");
        return 1;
    }
    if (rename(argv[1], argv[2]) != 0) {
        perror("mv");
        return 1;
    }
    return 0;
}

int my_cat(int argc, char **argv)
{
    FILE *fp;
    int c, result = 0;

    if (argc == 1) {
        while ((c = getchar()) != EOF) {
            if (putchar(c) == EOF) {
                perror("cat: write");
                return 1;
            }
        }
        if (ferror(stdin)) {
            perror("cat: read");
            result = 1;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            fp = fopen(argv[i], "r");
            if (fp == NULL) {
                perror(argv[i]);
                result = 1;
                continue;
            }
            while ((c = fgetc(fp)) != EOF) {
                if (putchar(c) == EOF) {
                    perror("cat: write");
                    fclose(fp);
                    return 1;
                }
            }
            if (ferror(fp)) {
                perror("cat: read");
                result = 1;
            }
            if (fclose(fp) != 0) {
                perror("cat: close");
                result = 1;
            }
        }
    }
    if (fflush(stdout) == EOF) {
        perror("cat: write");
        result = 1;
    }
    return result;
}

int my_grep(int argc, char **argv)
{
    FILE *fp = stdin;
    char *line = NULL;
    size_t size = 0;
    int matched = 0, result = 0;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: grep pattern [file]\n");
        return 2;
    }
    if (argc == 3) {
        fp = fopen(argv[2], "r");
        if (fp == NULL) {
            perror(argv[2]);
            return 2;
        }
    }
    /* 한 줄 전체를 읽어서 긴 줄도 중간에 잘리지 않게 검색한다. */
    while (getline(&line, &size, fp) != -1) {
        if (strstr(line, argv[1]) != NULL) {
            matched = 1;
            if (fputs(line, stdout) == EOF) {
                perror("grep: write");
                result = 2;
                break;
            }
        }
    }
    if (ferror(fp)) {
        perror("grep: read");
        result = 2;
    }
    free(line);
    if (fp != stdin && fclose(fp) != 0) {
        perror("grep: close");
        result = 2;
    }
    if (fflush(stdout) == EOF) {
        perror("grep: write");
        result = 2;
    }
    return result != 0 ? result : (matched ? 0 : 1);
}
