#include <mpi.h> 
#include <stdio.h>

int main(int argc, char** argv) { 
    int rank, size, number;

MPI_Init(&argc, &argv); 
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

if (rank == 0) {
number = 42; // Initialized value }

MPI_Bcast(&number, 1, MPI_INT, 0, MPI_COMM_WORLD);

printf("Process %d received number %d\n", rank, number);

MPI_Finalize(); 
return 0;
} 