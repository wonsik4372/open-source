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


