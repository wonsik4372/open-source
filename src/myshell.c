#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mycmd.h"

int getargs(char *cmd, char **argv);
void run(int narg, char **argv);

int main()
{
    char buf[256];
    char *argv[50];
    int narg, bg;
    pid_t pid;

    while (1) {
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;

        printf("shell> ");
        fflush(stdout);
        if (fgets(buf, 256, stdin) == NULL)
            break;
        buf[strlen(buf) - 1] = '\0';

        narg = getargs(buf, argv);
        if (narg == 0)
            continue;

        if (strcmp(argv[0], "exit") == 0) {
            printf("쉘을 종료합니다\n");
            break;
        }
        if (strcmp(argv[0], "cd") == 0) {
            my_cd(narg, argv);
            continue;
        }

        bg = 0;
        if (strcmp(argv[narg - 1], "&") == 0) {
            bg = 1;
            argv[narg - 1] = NULL;
            narg--;
        }

        pid = fork();
        if (pid == 0) {
            run(narg, argv);
        } else if (pid > 0) {
            if (bg == 1)
                printf("[%d] 백그라운드 실행\n", pid);
            else
                waitpid(pid, NULL, 0);
        } else
            perror("fork failed");
    }
    return 0;
}

void run(int narg, char **argv)
{
    if (strcmp(argv[0], "ls") == 0) exit(my_ls(narg, argv));
    if (strcmp(argv[0], "pwd") == 0) exit(my_pwd(narg, argv));
    if (strcmp(argv[0], "mkdir") == 0) exit(my_mkdir(narg, argv));

    execvp(argv[0], argv);
    perror("execvp");
    exit(1);
}

int getargs(char *cmd, char **argv)
{
    int narg = 0;

    while (*cmd) {
        if (*cmd == ' ' || *cmd == '\t')
            *cmd++ = '\0';
        else {
            argv[narg++] = cmd++;
            while (*cmd != '\0' && *cmd != ' ' && *cmd != '\t')
                cmd++;
        }
    }
    argv[narg] = NULL;
    return narg;
}
