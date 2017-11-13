#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int countErrors(int *array_of_errcodes,int size);
int countErrors(int *array_of_errcodes, int size){
  int error_count=0,i;
  for(i=0;i<size;i++){
    if(array_of_errcodes[i]!=MPI_SUCCESS){
      error_count++;
    }
  }
return error_count;
}
void printMatrix(float **M, int lines, int cols){
  int i,j;
  for(i=0;i<cols;i++){
    printf("\nColuna %d :\n",i);
    for(j=0;j<lines;j++){
      printf("[%f]\n",M[i][j]);
    }
    printf("\n");
  }
}
