#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "panic_log.h"
#include "simulator.h"
#include "particle.h"

FILE* dataOpenNewFile(const char* fileName)
{
	file = fopen(fileName, "w");
	
	if (file == NULL)
	{
		logPrint(logHandle, LOG_ARGS, "DATA", LOG_ERR, "fopen: %s",
					strerror(errno));
		EXITF;
	}
	
	fprintf(file, "%d %d %d %d", totalPartTypes[0], totalPartTypes[1],
			  totalPartTypes[2], totalPartTypes[3]);
}

void dataStartIteration()
{
	fprintf(file, "\n%d ", iteration);
}

void dataWriteParticle(int part)
{
	fprintf(file, "%f %f %f %f %f %f ",
			  parts[part].coords[0], parts[part].coords[1], parts[part].coords[2],
			  parts[part].vel[0], parts[part].vel[1], parts[part].vel[2]);
}
