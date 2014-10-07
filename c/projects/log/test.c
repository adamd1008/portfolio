#include <stdio.h>
#include <string.h>
#include "log.h"

int main()
{
	log_t log;
	
	logInit(&log, NULL, LOG_ALL, LOG_FLAG_SRC_INFO, single);
	
	logPrint(log, LOG_ARGS, LOG_INFO, 0, "This is a test message: %s",
				"Hello world!");
	
	logPrint(log, LOG_ARGS, LOG_INFO, 0, "Another one!");
	
	char str[128];
	strcpy(str, "This is a test.\nHello, how are you doing.\n\tIt's a very nice day today, of course it is.\tLel scrubs\n");
	
	logHexdumpz(log, LOG_ARGS, 0, str);
	
	logCleanup(log);
	
	return 0;
}
