#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 4  // Macro for the size of the arrays 

int main(int argc, char *argv[]){

    int rank;

    MPI_Init(&argc, &argv);  // Initialize MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the current process

    int array1[SIZE] = {1, 2, 3, 4};  // Array to be processed by Process 1
    int array2[SIZE] = {5, 6, 7, 8};  // Array to be processed by Process 2
    int result1[SIZE];               // Will hold squared values from Process 1
    int result2[SIZE];               // Will hold doubled values from Process 2

    if(rank == 0){

        // Master process sends array1 to Process 1 (tag 10)
        MPI_Send(array1, SIZE, MPI_INT, 1, 10, MPI_COMM_WORLD);

        // Master process sends array2 to Process 2 (tag 20)
        MPI_Send(array2, SIZE, MPI_INT, 2, 20, MPI_COMM_WORLD);
    } 
    else if(rank == 1){

        int recv_arr[SIZE];
        MPI_Status status;

        // Process 1 receives array1 from Process 0 (tag 10)
        MPI_Recv(recv_arr, SIZE, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);

        // Process 1 squares each element
        for(int i = 0; i < SIZE; i++){
            recv_arr[i] = recv_arr[i] * recv_arr[i];
        }

        // Sends squared result to Process 3 (tag 100)
        MPI_Send(recv_arr, SIZE, MPI_INT, 3, 100, MPI_COMM_WORLD);
    } 
    else if(rank == 2){

        int recv_arr[SIZE];
        MPI_Status status;

        // Process 2 receives array2 from Process 0 (tag 20)
        MPI_Recv(recv_arr, SIZE, MPI_INT, 0, 20, MPI_COMM_WORLD, &status);

        // Process 2 doubles each element
        for(int i = 0; i < SIZE; i++){
            recv_arr[i] = recv_arr[i] * 2;
        }

        // Sends doubled result to Process 3 (tag 200)
        MPI_Send(recv_arr, SIZE, MPI_INT, 3, 200, MPI_COMM_WORLD);
    } 
    else if(rank == 3){

        MPI_Status status;
        int recv_arr[SIZE];

        // Process 3 will receive results from both Process 1 and 2
        for(int i = 0; i < 2; i++){

            // Receives from any source and distinguishes by tag
            MPI_Recv(recv_arr, SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // If tag is 100, it's from Process 1 (squared data)
            if(status.MPI_TAG == 100){
                for(int j = 0; j < SIZE; j++){
                    result1[j] = recv_arr[j];
                }
            } 
            // If tag is 200, it's from Process 2 (doubled data)
            else if(status.MPI_TAG == 200){
                for(int j = 0; j < SIZE; j++){
                    result2[j] = recv_arr[j];
                }
            }
        }

        // Print the final combined result (squared + doubled values)
        printf("Final Aggregated Result:\n");

        for(int i = 0; i < SIZE; i++){

            int final;
            final = result1[i] + result2[i];
            printf("%d ", final);
        }

        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
