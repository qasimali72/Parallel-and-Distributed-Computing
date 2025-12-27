include <stdio.h> 
#include <omp.h> 

int main() { 
    int array1[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; 
    int array2 [16] {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}; 
    int result1 = 0,result2 =0


    #pragma omp parallel num_threads(16)
    {
        #pragma omp for
        for(int i=0; i< 16; i++){
            #pragma omp atomic
            result1 +=array1[i];
        }
        #pragma omp section
        {
            if(result1 > 10){
                #pragma omp parallel for
                for(int i=0; i<16; i++){
                    #pragma omp atomic
                    result2 +:array2[i];
                }
            }
        }
    }

    printf("%d", result2);
   
return 0;
}

