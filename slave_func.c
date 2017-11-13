#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define SUCCESS 1
#define ERROR 0

void subtract(float* cols, float *multipliers_array, int myrank, int pivot_column,int pivot_line, int col_count, int col_size);
float* getMultipliers(float *cols,int col_size,int pivot_line,int pivot_column);
void changeRows(float *cols, int pivot_line, int target_index, int pivot_column, int col_count, int order, float *multipliers_array);
int findPivot(float *cols, int col_size, int col, int *index, int target_line);

int findPivot(float *cols, int col_size, int col, int *index, int target_line){
  int row;
  for (row=target_line;row<col_size;row++){
    if(cols[col*col_size+row]!=0){
      *index = row;
      return SUCCESS;
    }
  }
  return ERROR;
}

void changeRows(float *cols, int pivot_line, int target_index, int pivot_column, int col_count, int order,
  float *multipliers_array){
  int i;
  float aux;
  for(i=pivot_column;i<col_count;i++){
    aux = cols[target_index+i*order];
    cols[target_index+i*order] = cols[pivot_line+i*order];
    cols[pivot_line+i*order]=aux;
  }
  aux = multipliers_array[pivot_line];
  multipliers_array[pivot_line] = multipliers_array[target_index];
  multipliers_array[target_index] = aux;
}

float* getMultipliers(float *cols,int col_size,int pivot_line,int pivot_column){
  int i;
  float pivot;
  float *multipliers_array;
  multipliers_array=(float*) malloc(sizeof(float)*col_size);
  pivot = cols[pivot_line+pivot_column*col_size];
  #pragma omp parallel for num_threads(1+col_size/500)
  for(i=0;i<col_size;i++){
    multipliers_array[i] = cols[i+pivot_column*col_size]/pivot;
  }
  multipliers_array[pivot_line] = pivot;
  return multipliers_array;
}

void subtract(float* cols, float *multipliers_array, int myrank, int pivot_column,int pivot_line, int col_count, int col_size){
  int i, k;
  float aux, pivot;
  for(i=pivot_column;i<col_count;i++){
    pivot = cols[pivot_line+i*col_size];
    #pragma omp parallel for num_threads(1+col_size/500)
    for(k=0;k<col_size;k++){
      if(k==pivot_line){
        cols[k+i*col_size] = cols[k+i*col_size]/multipliers_array[k];
      }
      else{
        aux = pivot*multipliers_array[k];
        cols[k+i*col_size] = cols[k+i*col_size] - aux;
      }
    }
  }
  return;
}
