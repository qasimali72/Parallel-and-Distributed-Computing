#include <stdio.h>
#include <mpi.h>

#define N 16  // Total number of elements to process

int main(int argc, char *argv[]) {
    int rank;
    int size;
    int arr[N], final_arr[N];

    MPI_Init(&argc, &argv);  // Initialize the MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get total number of processes

    // Determine base number of elements per process and remainder
    int q = N / size;
    int r = N % size;

    if (rank == 0) {

        // Master process collects input from the user
        printf("Enter %d elements:\n", N);
        for (int i = 0; i < N; i++) {
            scanf("%d", &arr[i]);
        }

        // Arrays for non-blocking send and receive operations
        MPI_Request send_requests[size - 1];
        MPI_Request recv_requests[size - 1];
        MPI_Status statuses[size - 1];

        // Calculate how many elements master will handle
        int local_n = (r > 0) ? q + 1 : q;

        // Master performs its own computation
        for (int i = 0; i < local_n; i++) {
            final_arr[i] = arr[i] * arr[i];
        }

        MPI_Barrier(MPI_COMM_WORLD);  // Synchronize all before timing
        double start = MPI_Wtime();   // Start timer

        // Distribute remaining data to worker processes using non-blocking sends and receives
        int offset = local_n;
        for (int j = 1; j < size; j++) {
            int send_count = (j < r) ? q + 1 : q;

            MPI_Isend(&arr[offset], send_count, MPI_INT, j, 0, MPI_COMM_WORLD, &send_requests[j - 1]);
            MPI_Irecv(&final_arr[offset], send_count, MPI_INT, j, 0, MPI_COMM_WORLD, &recv_requests[j - 1]);

            offset += send_count;
        }

        // Wait for all send and receive operations to complete
        if (size > 1) {
            MPI_Waitall(size - 1, send_requests, statuses);
            MPI_Waitall(size - 1, recv_requests, statuses);
        }

        MPI_Barrier(MPI_COMM_WORLD);  // Synchronize again before ending the timer
        double end = MPI_Wtime();     // Stop timer

        // Display the final squared array
        printf("Squared array:\n");
        for (int i = 0; i < N; i++) {
            printf("%d ", final_arr[i]);
        }
        printf("\n");

        // Print total time taken for the non-blocking communication and computation
        printf("Non-blocking time: %f seconds\n", end - start);
    } 
    else {

        // Worker processes calculate how many elements they will receive
        int local_n = (rank < r) ? q + 1 : q;
        int local_arr[local_n];
        int squared_arr[local_n];

        MPI_Request recv_req;
        MPI_Request send_req;

        MPI_Barrier(MPI_COMM_WORLD);  // Sync with master before receiving data

        // Non-blocking receive from master
        MPI_Irecv(local_arr, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, &recv_req);
        MPI_Wait(&recv_req, MPI_STATUS_IGNORE);  // Wait until data is received

        // Perform square operation on received data
        for (int i = 0; i < local_n; i++) {
            squared_arr[i] = local_arr[i] * local_arr[i];
        }

        // Send the squared result back to master using non-blocking send
        MPI_Isend(squared_arr, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, &send_req);
        MPI_Wait(&send_req, MPI_STATUS_IGNORE);  // Wait until data is sent

        MPI_Barrier(MPI_COMM_WORLD);  // Final sync
    }

    MPI_Finalize();  // Finalize the MPI environment
    return 0;
}
