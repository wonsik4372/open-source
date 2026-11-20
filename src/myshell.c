#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mycmd.h"

int getargs(char *cmd, char **argv);
void run(int narg, char **argv);
void handler(int signo);

pid_t fgpid = 0;

int main()
{
    char buf[256];
    char *argv[50];
    char *argv2[50];
    int narg, i, j, bg, p[2], status;
    pid_t pid, pid2;
    struct sigaction act;

    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);

    act.sa_handler = handler;
    sigfillset(&(act.sa_mask));
    act.sa_flags = 0;
    sigaction(SIGTSTP, &act, NULL);

    while (1) {
        while (waitpid(-1, NULL, WNOHANG) > 0)
            ;

        printf("shell> ");
        fflush(stdout);
        if (fgets(buf, 256, stdin) == NULL) {
            if (feof(stdin))
                break;
            clearerr(stdin);
            printf("\n");
            continue;
        }
        buf[strlen(buf) - 1] = '\0';

        narg = getargs(buf, argv);
        if (narg == 0)
            continue;

        if (strcmp(argv[0], "exit") == 0)
            break;

        bg = 0;
        if (strcmp(argv[narg - 1], "&") == 0) {
            bg = 1;
            argv[narg - 1] = NULL;
            narg--;
        }

        for (i = 0; i < narg; i++) {
            if (strcmp(argv[i], "|") == 0)
                break;
        }

        if (i < narg) {
            argv[i] = NULL;
            for (j = 0; argv[i + 1 + j] != NULL; j++)
                argv2[j] = argv[i + 1 + j];
            argv2[j] = NULL;

            pipe(p);
            pid = fork();
            if (pid == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                dup2(p[1], 1);
                close(p[0]);
                close(p[1]);
                run(i, argv);
            }
            pid2 = fork();
            if (pid2 == 0) {
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                dup2(p[0], 0);
                close(p[0]);
                close(p[1]);
                run(j, argv2);
            }
            close(p[0]);
            close(p[1]);
            waitpid(pid, NULL, 0);
            waitpid(pid2, NULL, 0);
            continue;
        }

        pid = fork();
        if (pid == 0) {
            if (!bg) {
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
            }
            run(narg, argv);
        } else if (pid > 0) {
            if (bg) {
                printf("[%d] 백그라운드 실행\n", pid);
            } else {
                fgpid = pid;
                while (waitpid(pid, &status, 0) == -1)
                    ;
                fgpid = 0;
                if (!WIFEXITED(status))
                    printf("\n");
            }
        } else
            perror("fork failed");
    }
    return 0;
}

void run(int narg, char **argv)
{
    int i, fd, n = narg;

    for (i = 0; i < narg; i++) {
        if (argv[i] == NULL)
            break;
        if (strcmp(argv[i], ">") == 0) {
            fd = open(argv[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            dup2(fd, 1);
            close(fd);
            argv[i] = NULL;
            if (i < n)
                n = i;
            i++;
        }
        else if (strcmp(argv[i], "<") == 0) {
            fd = open(argv[i + 1], O_RDONLY);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, 0);
            close(fd);
            argv[i] = NULL;
            if (i < n)
                n = i;
            i++;
        }
    }
    narg = n;

    if (strcmp(argv[0], "rmdir") == 0) exit(my_rmdir(narg, argv));
    if (strcmp(argv[0], "ln") == 0) exit(my_ln(narg, argv));
    if (strcmp(argv[0], "cp") == 0) exit(my_cp(narg, argv));
    if (strcmp(argv[0], "rm") == 0) exit(my_rm(narg, argv));

    execvp(argv[0], argv);
    perror("execvp");
    exit(1);
}

void handler(int signo)
{
    if (fgpid > 0) {
        kill(fgpid, SIGKILL);
        printf("\n[%d] Ctrl-Z 로 종료", fgpid);
    }
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
