#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// Macro to define the total number of input elements
#define TOTAL_NUMBERS 16

int main(int argc, char *argv[]){

    int rank, num_processes;
    int data[TOTAL_NUMBERS];    // Only used by master to store input
    int result[TOTAL_NUMBERS];  // Only used by master to store output

    // Initialize MPI environment
    MPI_Init(&argc, &argv);

    // Get the rank of the current process
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get the total number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    // Make sure the number of processes does not exceed data elements
    if(num_processes > TOTAL_NUMBERS){

        if(rank == 0){
            // Only the master prints this error
            printf("Only up to %d processes are allowed.\n", TOTAL_NUMBERS);
        }

        MPI_Finalize();
        return 0;
    }

    int base = TOTAL_NUMBERS / num_processes; // Minimum number of elements per process
    int extra = TOTAL_NUMBERS % num_processes; // Remainder elements to distribute

    // Master process logic
    if(rank == 0){

        printf("Enter %d numbers:\n", TOTAL_NUMBERS);

        // Read all numbers from user input
        for(int i = 0; i < TOTAL_NUMBERS; i++){
            scanf("%d", &data[i]);
        }

        // Arrays to manage non-blocking communication
        MPI_Request send_requests[num_processes - 1];
        MPI_Request recv_requests[num_processes - 1];
        MPI_Status statuses[num_processes - 1];

        int current_index = 0;  // Index for traversing input array
        int result_index = 0;   // Index for filling result array

        int local_count;

        // Master determines how many elements it will process
        if(extra > 0){
            local_count = base + 1;
        }
        else{
            local_count = base;
        }

        // Master squares its portion of the array
        for(int i = 0; i < local_count; i++){
            result[result_index] = data[current_index] * data[current_index];
            current_index++;
            result_index++;
        }

        int req_index = 0;

        // Send chunks to worker processes using non-blocking sends/receives
        for(int i = 1; i < num_processes; i++){

            int send_count;

            if(i < extra){
                send_count = base + 1; // Give extra elements to early processes
            }
            else{
                send_count = base;
            }

            // Start non-blocking send and receive for each worker
            MPI_Isend(&data[current_index], send_count, MPI_INT, i, 0, MPI_COMM_WORLD, &send_requests[req_index]);
            MPI_Irecv(&result[result_index], send_count, MPI_INT, i, 0, MPI_COMM_WORLD, &recv_requests[req_index]);

            current_index += send_count;
            result_index += send_count;
            req_index++;
        }

        // Wait for all sends and receives to complete
        if(num_processes > 1){
            MPI_Waitall(num_processes - 1, send_requests, statuses);
            MPI_Waitall(num_processes - 1, recv_requests, statuses);
        }

        // Display final results
        printf("Final squared array:\n");

        for(int i = 0; i < TOTAL_NUMBERS; i++){
            printf("%d ", result[i]);
        }

        printf("\n");
    }

    // Worker process logic
    else{

        int local_count;

        // Determine how many elements this process will handle
        if(rank < extra){
            local_count = base + 1;
        }
        else{
            local_count = base;
        }

        int recv_data[local_count];     // Buffer to receive data from master
        int squared_data[local_count];  // Buffer to store squared results

        MPI_Request recv_req;
        MPI_Request send_req;

        // Receive chunk of data from master (non-blocking)
        MPI_Irecv(recv_data, local_count, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_req);
        MPI_Wait(&recv_req, MPI_STATUS_IGNORE); // Wait for data to arrive

        // Square each received number
        for(int i = 0; i < local_count; i++){
            squared_data[i] = recv_data[i] * recv_data[i];
        }

        // Send back the results to master (non-blocking)
        MPI_Isend(squared_data, local_count, MPI_INT, 0, 0, MPI_COMM_WORLD, &send_req);
        MPI_Wait(&send_req, MPI_STATUS_IGNORE); // Wait for send to complete
    }

    MPI_Finalize(); // Clean up the MPI environment
    return 0;
}
