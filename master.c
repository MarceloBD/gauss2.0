#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include "master_func.c"
#include "slave_func.c"
#include "file.c"
#define SUCCESS 1
#define ERROR 0
#define NUMBER_OF_CLUSTER_NODES 3


void printM(float *matrix, int row, int col){
  int i, j, my_rank;
 MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  for(i=0;i<row;i++){
    for(j=0;j<col;j++){
      printf("%f _%d_", matrix[i+j*row], my_rank);
    }
    printf("\n" );
  }
}

int main(int argc, char *argv[])
{
  int i,j, my_rank,num_proc,num_slaves, *col_count, *elem_count;
  col_count = (int*)malloc(sizeof(int));
  elem_count =   (int*)malloc(sizeof(int));
  int divis,r;
  int *order = (int*)malloc(sizeof(int));
  //*order = 10;
  int *sendcounts;
  int *displs;
  /*AB is an array of columns representing the matrix */
  float *AB, *cols;
  int *array_of_errcodes;
  MPI_Datatype column_type, col;
  MPI_Comm  inter_comm;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_proc);

  if(my_rank==0){
    /* Allocate matrix. N rows and N+1 columns */
//    AB = (float*) malloc(((*order)+1)*(*order)*(sizeof(float) ));
    /*for(i = 0;i<(*order+1)*(*order);i++){
      AB[i] = rand()%10 ;
     }
     AB[0]= 4;
      AB[1]= 5;
       AB[2]=1;
        AB[3]= 2;
         AB[4]= 3;
          AB[5]= 6;
           AB[6]= 3 ;
            AB[7]=6;
             AB[8]= 7;
              AB[9]= 4;
               AB[10]= 6 ;
                AB[11]=2;
                 AB[12]= 1;

*/
  FILE *f_matrix, *f_vector;
  char matrix_name[] = "matriz.txt";
  char vector_name[]= "vetor.txt";
  //int order=0, order_v;

  f_matrix = openFile(matrix_name);
  f_vector = openFile(vector_name);

  readF(&AB, f_matrix, f_vector, order);
  closeFile(f_matrix);
  closeFile(f_vector);

    printM(AB, *order, *order+1);

    sendcounts= (int*) malloc(sizeof(int)*num_proc);
    displs= (int*) malloc(sizeof(int)*num_proc);
    printf("numproc = %d\n",num_proc );

    divis=(*order+1)/num_proc;
    r=(*order+1)%num_proc;

    for(i=0;i<num_proc;i++){
      if(i<r){
        sendcounts[i]=(divis+1)*(*order);
      }
      else{
        sendcounts[i]=divis*(*order);
      }
    }
    for(i=0;i<num_proc;i++){
      printf("Sendcounts[%d]: %d \n",i,sendcounts[i]);
    }
    displs[0]=0;
    printf("displs[0]: %d \n",displs[0]);
    for(i=1;i<num_proc;i++){
      displs[i]=sendcounts[i-1]+displs[i-1];
      //displs[i]=0;
      printf("displs[%d]: %d \n",i,displs[i]);
    }
  }
  //  int *test = (int*)malloc(sizeof(int));
    MPI_Bcast(order,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Scatter(sendcounts,1,MPI_INT,elem_count,1,MPI_INT,0,MPI_COMM_WORLD);
  //  col_count = *test;
  //  elem_count = *test;
    *col_count = *elem_count/(*order);
    printf("%d my rank %d %d\n",*col_count ,my_rank, *order );
    //printf("%d\n", elem_count);
    /*Allocate memory chunk for array of columns */
    cols = (float*)malloc(*col_count*(*order)*sizeof(float));

    MPI_Scatterv(AB,sendcounts,displs,MPI_FLOAT,cols,*elem_count,MPI_FLOAT,0,MPI_COMM_WORLD);


//    printM(cols, *order, *col_count);
  //  MPI_Finalize();
  //  return 0;

  //  printM(cols, *order, *col_count);

    /********* Begginig of gauss algorithm ***************/

    int *target_line, *token, IS_LAST_PROC=0, *pivot_found,* pivot_line, *pivot_column;
    pivot_column   = (int*)malloc(sizeof(int));
    target_line =  (int*)malloc(sizeof(int));
    *pivot_column = 0;
    pivot_line  = (int*)malloc(sizeof(int));
    pivot_found  = (int*)malloc(sizeof(int));
    token =   (int*)malloc(sizeof(int));
    *token = 0;

    float *multipliers_array = (float*)malloc(*order*sizeof(float));
    /*Iterates over all rows of the matrix */
    *target_line=0;

    for((*target_line)=0;(*target_line)<(*order)  ;(*target_line)++){
      if(my_rank==0)
      printf(">>token token token token <<%d\n",*token );
      /* Search for pivot inside the column array */
      if(*token==my_rank){
        *pivot_found=ERROR;
        /*In the last process we don't want to process the last column of the matrix (as it is the expanded one)*/
        if(my_rank==num_proc-1){
          IS_LAST_PROC=1;
        }

        for(i=*pivot_column;i<(*col_count)-IS_LAST_PROC;i++){
          // if(*token==1){
          //   printf("PIVOT COLUMN %d col_count %d\n", *pivot_column, *col_count);
          //   MPI_Finalize();
          //
          //   return 0;}
          if(findPivot (cols, *order, i, pivot_line, *target_line) == SUCCESS){

            *pivot_found=SUCCESS;
            *pivot_column=i;
            multipliers_array = getMultipliers(cols,*order,*target_line,i );
            break;
          }
        }
        if(!*pivot_found){
          printf("Passing token onward from process %d token %d",num_proc,*token);
          (*token)++;
        }

        /*Change this to struct*/
        MPI_Bcast(pivot_line,1,MPI_INT,my_rank,MPI_COMM_WORLD);
        MPI_Bcast(token,1,MPI_INT,my_rank,MPI_COMM_WORLD);
    //    MPI_Bcast(pivot_column,1,MPI_INT,my_rank,MPI_COMM_WORLD);
        MPI_Bcast(multipliers_array,*order,MPI_FLOAT,my_rank,MPI_COMM_WORLD);
      }
      else{
        /*Change this to struct*/
         MPI_Bcast(pivot_line,1,MPI_INT,*token,MPI_COMM_WORLD);
         MPI_Bcast(token,1,MPI_INT,*token,MPI_COMM_WORLD);
      //   MPI_Bcast(pivot_column,1,MPI_INT,*token,MPI_COMM_WORLD);
         MPI_Bcast(multipliers_array,*order,MPI_FLOAT,*token,MPI_COMM_WORLD);


      }

      printf("target line %d pivot_column %d, col_count %d \n",*target_line, *pivot_column, *col_count );
      if(my_rank>=*token)
        changeRows(cols,*pivot_line,*target_line,*pivot_column,*col_count, *order, multipliers_array);


      if(my_rank>=*token)
        subtract(cols,multipliers_array,my_rank,*pivot_column,*target_line,*col_count,*order);
  //    printM(cols, *order, *col_count);
//      (*target_line)++;

      if(my_rank==1)
      printM(multipliers_array, *order,1);

      if(my_rank==*token){
        (*pivot_column)++;
        if(*pivot_column==*col_count){
          *pivot_column =0;
          (*token)++;
        }
          MPI_Bcast(token,1,MPI_INT,my_rank,MPI_COMM_WORLD);

      }
      else
        MPI_Bcast(token,1,MPI_INT,*token,MPI_COMM_WORLD);


    }
    MPI_Gatherv(cols,*elem_count,MPI_FLOAT,AB,sendcounts,displs,MPI_FLOAT,0,MPI_COMM_WORLD);
      if(my_rank==0){
      printM(AB, *order, *order+1);

      FILE *f_out;
      f_out = openFile("output.txt");
      //void writeF(char* data, int size, FILE* file, char *type){
      writeF(AB, *order, f_out, "end");
      closeFile(f_out);

    }



    MPI_Finalize();
    return 0;
}
