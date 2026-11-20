#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
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

int my_cp(int argc, char **argv) {
    int src, dst;
    int nread;
    char data[BUFSIZE];
    struct stat s1, s2;

    if(argc != 3) {
        fprintf(stderr, "cp: cp 원본 대상\n");
        return 1;
    }
    src = open(argv[1], O_RDONLY);
    if(src < 0) {
        perror(argv[1]);
        return 1;
    }
    fstat(src, &s1);

    /* 대상이 이미 있고 원본과 같은 파일이면 중단 (O_TRUNC로 내용이 날아가는 것 방지) */
    if(stat(argv[2], &s2) == 0 &&
    s1.st_dev == s2.st_dev && s1.st_ino == s2.st_ino) {
        fprintf(stderr, "cp: 원본과 대상이 같은 파일입니다\n");
        close(src);
        return 1;
    }

    /* 대상 열기: 없으면 생성, 있으면 비움, 권한은 원본과 동일 */
    dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, s1.st_mode & 0777);
    if(dst < 0) {
        perror(argv[2]);
        close(src);
        return 1;
    }

    /* BUFSIZE씩 읽어서 그대로 쓰기 (끝까지 반복) */
    while((nread = read(src, data, BUFSIZE)) > 0) {
        if(write(dst, data, nread) != nread) {
            perror("write");
            close(src); close(dst);
            return 1;
        }
    }
    if(nread < 0) {
        perror("read");
        close(src); close(dst);
        return 1;
    }
    close(src);
    close(dst);
    return 0;
}

int my_rm(int argc, char **argv) {
    int i;
    int ret = 0;                            /* 실패 여부 기록 */

    if(argc < 2) {
        fprintf(stderr, "rm: 파일 이름을 입력하세요\n");
        return 1;
    }
    for(i = 1; i < argc; i++) {
        if(unlink(argv[i]) < 0) {           /* 실패해도 나머지는 계속 삭제 */
            perror(argv[i]);
            ret = 1;
        }
    }
    return ret;
}