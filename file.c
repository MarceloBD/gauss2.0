#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include <ctype.h>
#define BUFFER_SIZE 100000

FILE *openFile(char* name){
    FILE *fp;
    if(!(fp = fopen(name, "r"))){              //verifies if the file already exists
        fp = fopen(name , "w+");
        fclose(fp);
    }
    return fopen(name, "r+");                  // returns the pointer to the file in both cases
}

int closeFile(FILE* fp){                     // closes file
    return fclose(fp);
}

void writeF(float* data, int size, FILE* file, char *type){       // write string in file
    if (type == "end"){
        fseek ( file , 0 , SEEK_END );                          // if type is end, seeks at end of file
    }
    else{
        fseek( file, *type, SEEK_SET);                           // else seeks in "type" byte offset
    }
    char print_buff[BUFFER_SIZE];
    char buffer[64];
    int i,j, pos=0;
    for(i=0;i<size;i++){
      int ret = snprintf(buffer, sizeof buffer, "%f", *data+i);
      if (ret < 0) {
          return;
      }
      if (ret > sizeof buffer) {
          return;
          /* Result was truncated - resize the buffer and retry.*/
      }
      for(j=0;j<8;j++){
        print_buff[j+pos*9] = buffer[j];
      }
      print_buff[8+pos*9] = '\n';
      pos++;
    }

    fwrite(print_buff, sizeof(char), size*9, file);                     // writes on file
}


void readF(float** data, FILE *matrixf, FILE *vectorf, int* order){
    char buffer[BUFFER_SIZE] = {0}, buffer2[BUFFER_SIZE] = {0};
    char bufferv[BUFFER_SIZE] = {0}, buffer3[BUFFER_SIZE] = {0};
    char *p = buffer;
    int i, count=0, position=0, col=0;

    fseek (vectorf,0,SEEK_SET);                               // seeks at beginning of file
    fread(&buffer, sizeof(char), BUFFER_SIZE, vectorf);          // reads from file
    while (*p) {                           // While there are more characters to process...
      if (isdigit(*p)) {                  // Upon finding a digit, ...
        long val = strtoll(p, &p, 10);    // Read a number, ...
        bufferv[position] = (float) val;
        position++;
      } else {
        if(*p=='\n')
          count  ++;
        p++;
      }
    }
    position=0;

    fseek (matrixf,0,SEEK_SET);                               // seeks at beginning of file
    fread(&buffer3, sizeof(char), BUFFER_SIZE, matrixf);          // reads from file

    p=buffer3;

    while (*p) {                              // While there are more characters to process...
      if (isdigit(*p)) {                      // Upon finding a digit, ...
        long val = strtoll(p, &p, 10);        // Read a number, ...
        buffer2[position*count+col] = (float) val;
        position++;
        //col++;
      } else {
        if(*p=='\n'){
          position = 0;
          col++;
        }
        p++;
      }
    }
    for(i=0;i<count;i++)
      buffer2[count*count+i]=bufferv[i];


    *data = (float*)malloc(sizeof(float)*(count+1)*count);         // allocates memory for data array
      for(i=0;i<count*(count+1);i++)
        (*data)[i] = (float)buffer2[i];
    //strcpy(*data, buffer2);                                      // copies buffer to data
    *order = count;
}
/*
void main(){

FILE *f_matrix, *f_vector;
char matrix_name[] = "matriz.txt";
char vector_name[]= "vetor.txt";
int order=0, order_v;

f_matrix = openFile(matrix_name);
f_vector = openFile(vector_name);

char *matrix_data;

readF(&matrix_data, f_matrix, f_vector, &order);

printf("order = %d\n", order);
int i, j;
for(i=0;i<order;i++){
  for(j=0;j<order+1;j++){
    printf("%d ", matrix_data[i+j*order]);
  }
    printf(" \n" );
}

FILE *out;
out = openFile("output.txt");
float dataout[5]={2,3,4,5,6};
//void writeF(char* data, int size, FILE* file, char *type){
writeF(dataout, order, out, "end");

closeFile(f_matrix);
closeFile(f_vector);

return;
}
*/
