/*
 * Pipes, the easy way, through a command.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char** argv)
{
	FILE* pipe;
	char* strings[4] = {"delta", "echo", "charlie", "test"};
	
	pipe = popen("sort", "w");
	
	for (int i = 0; i < 4; i++)
	{
		fputs(strings[i], pipe);
		fputc('\n', pipe);
	}
	
	pclose(pipe);
	
	return 0;
}
