#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define TOTAL_NUMBERS 16 // Total number of integers to process

int main(int argc, char *argv[]){
	
	int rank, num_processes;
	int data[TOTAL_NUMBERS];     // Full array of numbers (handled only by master process)
	int result[TOTAL_NUMBERS];   // Final array to store squared numbers (filled by master)
	
	MPI_Init(&argc, &argv); // Start the MPI environment
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Get the current process ID
	MPI_Comm_size(MPI_COMM_WORLD, &num_processes); // Get the total number of processes
	
	if(num_processes > TOTAL_NUMBERS){
		
		if(rank == 0){
			// Too many processes compared to the number of elements
			printf("Error: Number of processes must be <= %d\n", TOTAL_NUMBERS);
		}
			
		MPI_Finalize();
		return 0;
	}
	
	int base = TOTAL_NUMBERS / num_processes; // Base number of elements each process should handle
	int extra = TOTAL_NUMBERS % num_processes; // Remaining elements to be distributed one-by-one to first few processes
	
	// Determine how many elements this process will handle
	int local_count;
	if(rank < extra){
		// First 'extra' processes get one additional element
		local_count = base + 1;
	}
	else{
		local_count = base;
	}
	
	// Compute the starting index for this process's data in the global array
	int start_index = 0;
	for(int i = 0; i < rank; i++){
		
		if(i < extra){
			start_index += base + 1; // Account for extra elements in earlier processes
		}
		else{
			start_index += base;
		}
	}
	
	int local_data[16];   // Buffer to hold this process's chunk of data
	int local_result[16]; // Buffer to hold squared results for this process
	
	// Master process will read the input and distribute chunks to all processes
	if(rank == 0){
		
		printf("Enter %d numbers:\n", TOTAL_NUMBERS);
		for(int i = 0; i < TOTAL_NUMBERS; i++){
			scanf("%d", &data[i]); // Input the numbers from user
		}
		
		// Distribute chunks of data to each worker process
		for(int i = 1; i < num_processes; i++){
			
			int send_count;
			if(i < extra){
				send_count = base + 1; // Give extra element to early processes
			}
			else{
				send_count = base;
			}
			
			// Calculate where this chunk begins in the data array
			int send_index = 0;
			for(int j = 0; j < i; j++){
				
				if(j < extra){
					send_index += base + 1;
				}
				else{
					send_index += base;
				}
			}
			
			// Send the appropriate chunk to the i-th process
			MPI_Send(&data[send_index], send_count, MPI_INT, i, 0, MPI_COMM_WORLD);
		}
		
		// Master computes squares of its own chunk
		for(int i = 0; i < local_count; i++){
			local_result[i] = data[i] * data[i];
		}
		
		// Copy master's results into the result array
		for(int i = 0; i < local_count; i++){
			result[i] = local_result[i];
		}
		
		// Collect the squared results from all worker processes
		int offset = local_count;
		for(int i = 1; i < num_processes; i++){
			
			int recv_count;
			if(i < extra){
				recv_count = base + 1;
			}
			else{
				recv_count = base;
			}
			
			// Receive results into the correct position in result array
			MPI_Recv(&result[offset], recv_count, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			offset += recv_count;
		}
		
		// Display the final squared result array
		printf("Final squared array:\n");
		for(int i = 0; i < TOTAL_NUMBERS; i++){
			printf("%d ", result[i]);
		}
		printf("\n");
	}
	
	// Worker processes: receive their chunk, square each number, and send result back
	else{
		
		MPI_Recv(local_data, local_count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		for(int i = 0; i < local_count; i++){
			local_result[i] = local_data[i] * local_data[i]; // Square each element
		}
		
		MPI_Send(local_result, local_count, MPI_INT, 0, 0, MPI_COMM_WORLD); // Send back to master
	}
	
	MPI_Finalize(); // Shut down MPI
	return 0;
}

