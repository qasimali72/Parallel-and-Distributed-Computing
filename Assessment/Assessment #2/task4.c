#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 4  // Number of processes in the ring
#define M 5  // Number of complete cycles

int main(int argc, char *argv[]){

    int rank;
    int num_processes;
    int counter;
    int terminate_flag;
    MPI_Status status;

    counter = 0;
    terminate_flag = 0;

    MPI_Init(&argc, &argv);  // Initialize MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);  // Get total number of processes

    // Check if enough processes are available to form a ring
    if(num_processes < N){

        if(rank == 0){
            printf("At least %d processes are required.\n", N);
        }

        MPI_Finalize();
        return 0;
    }

    // Logic for Process 0 (initiator and controller)
    if(rank == 0){

        printf("Process %d: Starting the counter at 0.\n", rank);

        // Start the ring by sending the initial counter to Process 1
        MPI_Send(&counter, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        // Loop to complete M cycles of counter passing
        for(int cycle = 0; cycle < M; cycle++){

            // Receive counter from the last process in the ring
            MPI_Recv(&counter, 1, MPI_INT, N - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            printf("Process %d: Received counter = %d from Process %d\n", rank, counter, status.MPI_SOURCE);

            // Increment the counter and send to the next process
            counter = counter + 1;
            MPI_Send(&counter, 1, MPI_INT, (rank + 1) % N, 0, MPI_COMM_WORLD);
        }

        // After M cycles, tell all other processes to terminate
        terminate_flag = 1;

        for(int i = 1; i < N; i++){
            MPI_Send(&terminate_flag, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
        }

        printf("Process %d: Terminating after %d cycles.\n", rank, M);
    } 
    // Logic for all other processes in the ring
    else{

        // Loop to receive and forward the counter during M cycles
        for(int cycle = 0; cycle < M; cycle++){

            // Receive counter from previous process in the ring
            MPI_Recv(&counter, 1, MPI_INT, (rank - 1 + N) % N, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            printf("Process %d: Received counter = %d from Process %d\n", rank, counter, status.MPI_SOURCE);

            // Increment and pass the counter to the next process
            counter = counter + 1;
            MPI_Send(&counter, 1, MPI_INT, (rank + 1) % N, 0, MPI_COMM_WORLD);
        }

        // After M cycles, wait for termination signal from Process 0
        MPI_Recv(&terminate_flag, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

        if(terminate_flag == 1){
            printf("Process %d: Terminating.\n", rank);
        }
    }

    MPI_Finalize();  // Finalize MPI environment
    return 0;
}
