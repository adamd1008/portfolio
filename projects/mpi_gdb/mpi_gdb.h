#ifndef MPI_GDB_H
#define MPI_GDB_H

#ifndef NDEBUG
#define initMPIGDB(x) mpiGDB(x)
#else
#define initMPIGDB(x) {}
#endif

void mpiGDB(int);

#endif
