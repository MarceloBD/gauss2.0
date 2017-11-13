#include <stdio.h>
#include <stdlib.h>
#define SUCCESS 1
#define ERROR 0

void subtract(float* cols, float *multipliers_array, int myrank, int pivot_column,int pivot_line, int col_count, int col_size);
float* getMultipliers(float *cols,int col_size,int pivot_line,int pivot_column);
void changeRows(float *cols, int pivot_line, int target_index, int pivot_column, int col_count, int order, float *multipliers_array);
int findPivot(float *cols, int col_size, int col, int *index, int target_line);
int findPivot(float *cols, int col_size, int col, int *index, int target_line){
  /*
     @param cols: the collums of the process
     @param col_size: number of lines
     @param col: index of the collum
     @param index: reference of the index of the pivot
     Find the first non zero element and return it's index
     or return -1 if all elements are zero
     */
  int row;

  for (row=target_line;row<col_size;row++){
//    if(cols[row][col] != 0){
  printf("rowwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwws = %d\n", row );
    if(cols[col*col_size+row]!=0){
      *index = row;
      printf("heeeeeeeeeeeeeeeere\n" );
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
    //aux = cols[target_index][i];
    aux = cols[target_index+i*order];
    //cols[target_index][i] = cols[pivot_line][i];
    cols[target_index+i*order] = cols[pivot_line+i*order];
    //cols[pivot_line][i] = aux;
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
  //pivot= cols[pivot_line][pivot_column];
  pivot = cols[pivot_line+pivot_column*col_size];
  for(i=0;i<col_size;i++){
    //multipliers_array[i]= cols[i][pivot_column]/pivot;
    multipliers_array[i] = cols[i+pivot_column*col_size]/pivot;
    printf("multipliers_array= %f %d col = %d pivo %f\n",multipliers_array[i], i, pivot_column, pivot );
  }
  multipliers_array[pivot_line] = pivot;
  return multipliers_array;
}

void subtract(float* cols, float *multipliers_array, int myrank, int pivot_column,int pivot_line, int col_count, int col_size){
  int i, k;
  float aux, pivot;
  for(i=pivot_column;i<col_count;i++){
    pivot = cols[pivot_line+i*col_size];
    for(k=0;k<col_size;k++){
      if(k==pivot_line){
        //cols[k][i] = cols[k][i]/multipliers_array[k];
        cols[k+i*col_size] = cols[k+i*col_size]/multipliers_array[k];
        printf("mult %f %f %d\n", multipliers_array[k], cols[0], k );
        printf("pivot line value :%f %d\n", cols[k+i*col_size], (k+i*col_size));
      }
      else{
        //aux = cols[pivot_line][i]*multipliers_array[k];
        aux = pivot*multipliers_array[k];
        printf("aux %f cols %f %d mult %f pivo %f\n", aux, cols[0], k, multipliers_array[k], pivot);
        //cols[k][i] = cols[k][i] - aux;
        cols[k+i*col_size] = cols[k+i*col_size] - aux;
      }
    }

  }
  return;
}
