#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void mpiGDB(int colSize)
{
   if (mpi_rank == 0)
      printf("finish\nfinish\nset var start = 1\nc\n");
   /* The master process will print out what I need to type into the xterms
      to start stepping */
   
   int myPID = getpid();
   int forkRes = fork();
   char str1[24];
   char str2[64];
   char str3[64];
   
   sprintf(str1, "mpi %.02d: xterm", mpi_rank);
   sprintf(str2, "Rank %.02d", mpi_rank);
   sprintf(str3, "gdb --pid %d", myPID);
   
   int column = (mpi_rank / colSize) * 506;
   int row = (mpi_rank % colSize) * 344 + 25;
   /* These vars make the xterms appear neatly on screen */
   
   char str4[32];
   
   sprintf(str4, "80x24+%d+%d", column, row);
   
   if (forkRes == 0)
   {
      if ((execlp("xterm", str1, "-geometry", str4, "-title", str2, "-e", str3,
                  NULL)) == -1)
      {
         perror("execlp");
         exit(EXIT_FAILURE);
      }
   }
   else if (forkRes == -1)
   {
      perror("fork");
      MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
   }
   
   int start = 0;
   
   while (start == 0)
      sleep(1);
}
