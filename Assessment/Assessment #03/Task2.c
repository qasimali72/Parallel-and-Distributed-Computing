#include <mpi.h> #include <stdio.h>

int main(int argc, char** argv)
 { int rank, size;
int data[16], recv_data[4];

MPI_Init(&argc, &argv); 
MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
MPI_Comm_size(MPI_COMM_WORLD, &size);

if (rank == 0) {

    for (int i = 0; i < 16; i++) { data[i] = i + 1;
    } 
}

MPI_Scatter(data, 4, MPI_INT, recv_data, 4, MPI_INT, 0, MPI_COMM_WORLD);

for (int i = 0; i < 4; i++) { 
    recv_data[i] *= 2;
    }

MPI_Gather(recv_data, 4, MPI_INT, data, 4, MPI_INT, 0, MPI_COMM_WORLD);

if (rank == 0) { 
    printf("Final array: ");
    for (int i = 0; i < 16; i++) { 
        printf("%d ", data[i]);
    } 
    printf("\n");
}

MPI_Finalize(); 
return 0;
} 