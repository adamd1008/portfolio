#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h> /* For PIPE_BUF */
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>

#define EXITF exit(EXIT_FAILURE)

int main(int argc, char** argv)
{
	if ((signal(SIGPIPE, SIG_IGN)) == SIG_ERR)
	{
		perror("signal");
		EXITF;
	}
	
	int pipeIn[2];
	int pipeOut[2];
	int pipeErr[2];
	
	if ((pipe(pipeIn)) == -1)
	{
		perror("pipe");
		EXITF;
	}
	
	if ((pipe(pipeOut)) == -1)
	{
		perror("pipe");
		EXITF;
	}
	
	if ((pipe(pipeErr)) == -1)
	{
		perror("pipe");
		EXITF;
	}
	
	pid_t child = fork();
	
	if (child == 0)
	{
		if ((dup2(pipeIn[0], STDIN_FILENO)) == -1)
		{
			perror("dup2");
			EXITF;
		}
		
		if ((dup2(pipeOut[1], STDOUT_FILENO)) == -1)
		{
			perror("dup2");
			EXITF;
		}
		
		if ((dup2(pipeErr[1], STDERR_FILENO)) == -1)
		{
			perror("dup2");
			EXITF;
		}
		
		if ((close(pipeIn[1])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((close(pipeOut[0])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((close(pipeErr[0])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((execlp("java", "java: minecraft server", "-Xmx1024M", "-Xms1024M",
						"-jar", "minecraft_server.jar", "nogui", NULL)) == -1)
		//if ((execlp("cat", "cat: test", NULL)) == -1)
		{
			perror("execlp");
			EXITF;
		}
	}
	else if (child == -1)
	{
		perror("fork");
		EXITF;
	}
	else
	{
		if ((close(pipeIn[0])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((close(pipeOut[1])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((close(pipeErr[1])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		/* Do stuff!! */
		
		fd_set activeSet;
		fd_set readSet;
		char buf[PIPE_BUF];
		char buf2[PIPE_BUF];
		
		/* Initialise the set of active FDs */
		
		FD_ZERO(&activeSet);
		FD_SET(STDIN_FILENO, &activeSet);
		FD_SET(pipeOut[0], &activeSet);
		FD_SET(pipeErr[0], &activeSet);
		
		int running = 1;
		
		while (running)
		{
			readSet = activeSet;
			
			if ((select(FD_SETSIZE, &readSet, NULL, NULL, NULL)) == -1)
			{
				perror("select");
				EXITF;
			}
			
			for (int i = 0; i < FD_SETSIZE; i++)
			{
				if (FD_ISSET(i, &readSet))
				{
					size_t charsR;
					size_t charsW;
					
					if (i == STDIN_FILENO)
					{
						/* The user has some input for the server child! */
						
						charsR = read(STDIN_FILENO, buf, PIPE_BUF);
						
						if (charsR == -1)
						{
							perror("read");
							EXITF;
						}
						else if (charsR == 0)
						{
							fprintf(stderr, "stdin returned 0\n");
							running = 0;
							break;
						}
						
						buf[charsR] = 0;
						
						memcpy(buf2, buf, charsR + 1);
						
						if (buf2[charsR - 1] == '\n')
						{
							buf2[charsR - 1] = '"';
							fprintf(stderr, "0> \"%s\n", buf2);
						}
						else
							fprintf(stderr, "0> \"%s\"\n", buf2);
						
						if ((charsW = write(pipeIn[1], buf, charsR)) == -1)
						{
							perror("write");
							EXITF;
						}
						
						if (charsR != charsW)
							fprintf(stderr, "0> 1 charsR != charsW\n");
					}
					else if (i == pipeOut[0])
					{
						charsR = read(pipeOut[0], buf, PIPE_BUF);
						
						if (charsR == -1)
						{
							perror("read");
							EXITF;
						}
						else if (charsR == 0)
						{
							fprintf(stderr, "stdout returned 0\n");
							running = 0;
							break;
						}
						
						buf[charsR] = 0;
						
						if (buf[charsR - 1] == '\n')
						{
							buf[charsR - 1] = '"';
							fprintf(stderr, "1> \"%s\n", buf);
						}
						else
							fprintf(stderr, "1> \"%s\"\n", buf);
					}
					else
					{
						charsR = read(pipeErr[0], buf, PIPE_BUF);
						
						if (charsR == -1)
						{
							perror("read");
							EXITF;
						}
						else if (charsR == 0)
						{
							fprintf(stderr, "stderr returned 0\n");
							running = 0;
							break;
						}
						
						buf[charsR] = 0;
						
						if (buf[charsR - 1] == '\n')
						{
							buf[charsR - 1] = '"';
							fprintf(stderr, "2> \"%s\n", buf);
						}
						else
							fprintf(stderr, "2> \"%s\"\n", buf);
					}
				}
			}
		}
		
		printf("Finishing\n");
		
		if ((close(pipeIn[1])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((close(pipeOut[0])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		if ((close(pipeErr[0])) == -1)
		{
			perror("close");
			EXITF;
		}
		
		/* End stuff!! */
	}
	
	return EXIT_SUCCESS;
}
