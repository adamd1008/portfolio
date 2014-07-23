#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/stat.h>

int errno;

int main(int argc, char** argv)
{
    char fifo_path[] = "/tmp/test.fifo";
    char buffer[128];

    int result = mkfifo(fifo_path, 0600);

    printf("mkfifo result = %d\n", result);

    if (errno == EEXIST)
        printf("errno == EEXIST\n");

    pid_t child = fork();

    if (child == 0)
    {
        printf("%d> child; opening fifo \"%s\" for writing\n", getpid(),
                 fifo_path);

        FILE* fifo = fopen(fifo_path, "w");
        char in_buffer[128];

        fgets(in_buffer, 128, stdin);

        fputs(in_buffer, fifo);

        fclose(fifo);
    }
    else
    {
        printf("%d> parent; opening fifo \"%s\" for reading\n", getpid(),
                 fifo_path);

        FILE* fifo = fopen(fifo_path, "r");

        fgets(buffer, 128, fifo);

        if (buffer[0] == EOF)
            printf("%d> got EOF\n", getpid());
        else
        {
            buffer[strlen(buffer) - 1] = 0;
            printf("%d> read string \"%s\"\n", getpid(), buffer);
        }

        fclose(fifo);
    }

    return 0;
}
