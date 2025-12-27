#include <mpi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h>

int main(int argc, char** argv) { 
    int rank, size, number;
int all_numbers[6]; 
int max_value; float average, total; 
double t1, t2;

MPI_Init(&argc, &argv); 
MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
MPI_Comm_size(MPI_COMM_WORLD, &size);

srand(time(NULL) + rank); 
number = rand() % 100 + 1;

t1 = MPI_Wtime();
MPI_Allgather(&number, 1, MPI_INT, all_numbers, 1, MPI_INT, MPI_COMM_WORLD); 
t2 = MPI_Wtime();

//if function 
if (rank == 0) { printf("All numbers: ");
for (int i = 0; i < size; i++) { 
    printf("%d ", all_numbers[i]);
}
printf("\nAllgather Time: %f seconds\n", t2 - t1);
}

t1 = MPI_Wtime();
MPI_Reduce(&number, &max_value, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD); 
t2 = MPI_Wtime();

//if this print 
if (rank == 0) {
printf("Maximum value: %d\n", max_value); 
printf("Reduce Time: %f seconds\n", t2 - t1);
}

//function call 
MPI_Allreduce(&number, &total, 1, MPI_FLOAT, MPI_SUM, MPI_COMM_WORLD); 
average = total / size;

//print ans
printf("Process %d: Average = %.2f\n", rank, average)

MPI_Finalize(); 
return 0;
} 