/*
 * Pipes, the not so easy way.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv)
{
	int fd[2];
	/*
	 * input is 0, output is 1
	 * 
	 * the fd that isn't required should be closed, i.e. if the process requires
	 * the input side of the pipe, close the output, and vice versa.
	 */
	
	char text[] = "Hello world!";
	char buffer[128];
	pid_t child;
	
	pipe(fd);
	
	child = fork();
	
	if (child == 0)
	{
		printf("%d> child; closing input fd(%d)\n", getpid(), fd[0]);
		close(fd[0]);
		
		write(fd[1], text, strlen(text) + 1);
	}
	else
	{
		printf("%d> parent; closing output fd(%d)\n", getpid(), fd[1]);
		close(fd[1]);
		
		read(fd[0], buffer, sizeof(buffer));
		
		printf("%d> read string \"%s\"\n", getpid(), buffer);
	}
	
	return 0;
}
