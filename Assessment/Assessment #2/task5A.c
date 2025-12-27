#include <stdio.h>
#include <mpi.h>

#define N 16  // Total number of elements in the array

int main(int argc, char *argv[]) {
    int rank;
    int size;
    int arr[N], final_arr[N];

    MPI_Init(&argc, &argv);  // Initialize MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);  // Get the rank of this process
    MPI_Comm_size(MPI_COMM_WORLD, &size);  // Get the total number of processes

    // Calculate base share and remainder for uneven division
    int q = N / size;
    int r = N % size;

    // Assign extra elements to first 'r' ranks
    int local_n = (rank < r) ? q + 1 : q;

    int local_arr[local_n];     // Array for received input chunk
    int squared_arr[local_n];   // Array for storing squared values

    if (rank == 0) {

        // Master process takes full input
        printf("Enter %d elements:\n", N);
        for (int i = 0; i < N; i++) {
            scanf("%d", &arr[i]);
        }

        MPI_Barrier(MPI_COMM_WORLD);  // Sync all processes before starting the timer
        double start = MPI_Wtime();   // Start the timer

        int offset = local_n;

        // Send chunks to each worker based on their local_n
        for (int i = 1; i < size; i++) {
            int send_count = (i < r) ? q + 1 : q;
            MPI_Send(&arr[offset], send_count, MPI_INT, i, 0, MPI_COMM_WORLD);
            offset += send_count;
        }

        // Master computes its portion
        for (int i = 0; i < local_n; i++) {
            squared_arr[i] = arr[i] * arr[i];
        }

        // Copy master's result to final array
        for (int i = 0; i < local_n; i++) {
            final_arr[i] = squared_arr[i];
        }

        offset = local_n;

        // Receive squared chunks back from workers
        for (int i = 1; i < size; i++) {
            int recv_count = (i < r) ? q + 1 : q;
            MPI_Recv(&final_arr[offset], recv_count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += recv_count;
        }

        MPI_Barrier(MPI_COMM_WORLD);  // Sync before stopping the timer
        double end = MPI_Wtime();     // Stop the timer

        // Output the squared array
        printf("Squared array:\n");
        for (int i = 0; i < N; i++) {
            printf("%d ", final_arr[i]);
        }
        printf("\n");

        // Display elapsed time for blocking communication and computation
        printf("Blocking time: %f seconds\n", end - start);
    } 
    else {
        // Workers wait until all processes are ready
        MPI_Barrier(MPI_COMM_WORLD);

        // Receive their chunk from master
        MPI_Recv(local_arr, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Square each received element
        for (int i = 0; i < local_n; i++) {
            squared_arr[i] = local_arr[i] * local_arr[i];
        }

        // Send squared result back to master
        MPI_Send(squared_arr, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD);

        MPI_Barrier(MPI_COMM_WORLD);  // Final sync before finishing
    }

    MPI_Finalize();  // Clean up the MPI environment
    return 0;
}
