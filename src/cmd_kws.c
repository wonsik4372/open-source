#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "mycmd.h"

#define BUFSIZE 4096

int my_rmdir(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "rmdir: 디렉토리 이름을 입력하세요\n");
        return 1;
    }
    if(rmdir(argv[1]) < 0) {
        perror("rmdir");
        return 1;
    }
    return 0;
}

int my_ln(int argc, char **argv) {
    int ret;

    if(argc == 4 && !strcmp(argv[1], "-s"))
        ret = symlink(argv[2], argv[3]);
    else if(argc == 3)
        ret = link(argv[1], argv[2]);
    else {
        fprintf(stderr, "ln: ln [-s] 원본 링크\n");
        return 1;
    }
    if(ret < 0) {
        perror("ln");
        return 1;
    }
    return 0;
}
