include <stdio.h> 
#include <omp.h> 

int main() { 
    int array1[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; 
    int array2 [16] {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; 
    int result[16]; 

//Get the total numnber of thread
int total_threads = omp_get_max_threads();

//use only half of the thread
int omp_get_thread_to_num = total_threads/2;

#pragma omp parallel for num_threads(omp_get_thread_to_use)
    for (int i=0; i < 16; i++) { 
    result[i] = array1[i] + array2[i]; 
    } 

    //dispaly the result
    for(int i=0; i<16; i++){
        printf("%d", result[i]);
    }
    printf("\n"); 
return 0;
}

