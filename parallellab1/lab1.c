#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>

/*** Skeleton for Lab 1 ***/

/***** Globals ******/
float **a; /* The coefficients */
float *x;  /* The unknowns */
float *b;  /* The constants */
float err; /* The absolute relative error */
int num = 0;  /* number of unknowns */


/****** Function declarations */
void check_matrix(); /* Check whether the matrix will converge */
void get_input();  /* Read input from file */

/********************************/



/* Function definitions: functions are ordered alphabetically ****/
/*****************************************************************/

/* 
   Conditions for convergence (diagonal dominance):
   1. diagonal element >= sum of all other elements of the row
   2. At least one diagonal element > sum of all other elements of the row
 */
void check_matrix()
{
  int bigger = 0; /* Set to 1 if at least one diag element > sum  */
  int i, j;
  float sum = 0;
  float aii = 0;
  
  for(i = 0; i < num; i++)
  {
    sum = 0;
    aii = fabs(a[i][i]);
    
    for(j = 0; j < num; j++)
       if( j != i)
   sum += fabs(a[i][j]);
       
    if( aii < sum)
    {
      printf("The matrix will not converge.\n");
      exit(1);
    }
    
    if(aii > sum)
      bigger++;
    
  }
  
  if( !bigger )
  {
     printf("The matrix will not converge\n");
     exit(1);
  }
}


/******************************************************/
/* Read input from file */
/* After this function returns:
 * a[][] will be filled with coefficients and you can access them using a[i][j] for element (i,j)
 * x[] will contain the initial values of x
 * b[] will contain the constants (i.e. the right-hand-side of the equations
 * num will have number of variables
 * err will have the absolute error that you need to reach
 */
void get_input(char filename[])
{
  FILE * fp;
  int i,j;  
 
  fp = fopen(filename, "r");
  if(!fp)
  {
    printf("Cannot open file %s\n", filename);
    exit(1);
  }

 fscanf(fp,"%d ",&num);
 fscanf(fp,"%f ",&err);

 /* Now, time to allocate the matrices and vectors */
 a = (float**)malloc(num * sizeof(float*));
 if( !a)
  {
  printf("Cannot allocate a!\n");
  exit(1);
  }

 for(i = 0; i < num; i++) 
  {
    a[i] = (float *)malloc(num * sizeof(float)); 
    if( !a[i])
    {
    printf("Cannot allocate a[%d]!\n",i);
    exit(1);
    }
  }
 
 x = (float *) malloc(num * sizeof(float));
 if( !x)
  {
  printf("Cannot allocate x!\n");
  exit(1);
  }


 b = (float *) malloc(num * sizeof(float));
 if( !b)
  {
  printf("Cannot allocate b!\n");
  exit(1);
  }

 /* Now .. Filling the blanks */ 

 /* The initial values of Xs */
 for(i = 0; i < num; i++)
  fscanf(fp,"%f ", &x[i]);
 
 for(i = 0; i < num; i++)
 {
   for(j = 0; j < num; j++)
     fscanf(fp,"%f ",&a[i][j]);
   
   /* reading the b element */
   fscanf(fp,"%f ",&b[i]);
 }
 
 fclose(fp); 

}


/************************************************************/

int sequential(){
    /*
    float **a; The coefficients
    float *x;  The unknowns 
    float *b;  The constants 
    float err; The absolute relative error
    int num = 0; number of unknowns
     */
    float unknowns[num];
    float testErr = 0;
    float currErr = 0;
    float maxErr = 0;
    float unknown = 0;
    int nit = 0;
    //looping thru all equations
    do{
        int i = 0;
        maxErr = 0;
        //creates a whole set of new unknowns
        for(i = 0; i < num; i++){
            unknown = b[i];
            int j = 0;
            //numerator calculation
            for(j = 0; j < num; j++){
                if(i != j){
                    unknown = unknown - a[i][j]*x[j];
                }
            }
            unknown = (unknown) / a[i][i];
            unknowns[i] = unknown;
            printf("%d: %f \n", i, unknown);
            //calculate err of current X
            printf("unknowns[i]: %f, x[i]: %f\n", unknowns[i], x[i]);
            currErr = ((float)(unknowns[i] - (float)x[i])/(float)(unknowns[i]));
            if(currErr < 0){
              //makes postive
              //currErr = ((float)(-1.0)*(unknowns[i] - (float)x[i])/(float)(unknowns[i]));
              if(-currErr > maxErr){
                maxErr = -currErr;
              }
            }
            else{
              if(currErr > maxErr){
                maxErr = currErr;
              }
            }
            printf("currentError: %f\n", currErr);
            //update testErr to be max of the set
        }
        //with each iteration, maxErr will get smaller
        //copy new unknowns to x[]
        memcpy(x, unknowns, sizeof(x) + sizeof(float));
        printf("MaxErr: %f\n", maxErr);
        testErr = maxErr;
        nit++;
    }while(testErr > err || testErr < (-1)*err);
    return nit;
}
 

int parallel(){
    /*
    float **a; The coefficients
    float *x;  The unknowns 
    float *b;  The constants 
    float err; The absolute relative error
    */
    int num = 0; number of unknowns
    int nit = 0;
    
    int comm_sz;
    int my_rank;
     
    MPI_Init(NULL, NULL);
    //all processes in comm world have access to stdout and sterr
    //only process 0 can access stdin
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    //comm sz is number of processors
    //my rank is current processor
    
    printf("Proc %d of %d:", my_rank, comm_sz);
    int quotient, my_n_count, my_first_i, my_last_i = 0;

    //figures out which cores get what jobs
    if(num % comm_sz == 0){
        quotient = num / comm_sz;
        my_n_count = quotient;
        my_first_i = my_rank * my_n_count;
        my_last_i = my_first_i + my_n_count;
    }
    else {
        quotient = num / comm_sz;
        remainder = num % comm_sz;
        if(my_rank < remainder){
            my_n_count = quotient + 1;
            my_first_i = my_rank * my_n_count;
        }
        else{
            my_n_count = quotient;
            my_first_i = my_rank * my_n_count + remainder;
        }
        my_last_i = my_first_i + my_n_count;
    }

    do{
      float maxErr = 0;
      float currErr = 0;
      if(my_rank != 0){
          int i = 0;
          for(i = my_first_i; i < my_last_i; i++){
              float unknown = b[i];
              int j = 0;
              //numerator calculation
              for(j = 0; j < num; j++){
                  if(i != j){
                      unknown = unknown - a[i][j]*x[j];
                  }
              }
              unknown = (unknown) / a[i][i];
              unknowns[i] = unknown;
              printf("%d: %f \n", i, unknown);
              //calculate err of current X
              printf("unknown[i]: %f, x[i]: %f\n", unknowns[i], x[i]);
              currErr = ((float)(unknowns[i] - (float)x[i])/(float)(unknowns[i]));
              printf("currentError: %f\n", currErr);
              //update testErr to be max of the set
              if(currErr > maxErr){
                  maxErr = currErr;
              }
              x[i] = unknowns[i];
          }
          MPI_Send(maxErr, sizeof(maxErr), MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
      } else {
      int q;
      float testErr;
      float newErr;
          for(q = 1; q < comm_sz; q++) {
              MPI_Recv(maxErr, sizeof(maxErr), MPI_FLOAT, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
              //find the largest error
              if(testErr < maxErr){
                 testErr = maxErr; 
              }
          }
          newErr = testErr;
          nit++;
      }
    } while(newErr > err || newErr < (-1)*err);
    MPI_Finalize();
    return nit;
    

    /*
    Pseudocode
    
    if (my_rank != 0)
        send maxErr to core 0
    else
        calculate unknown with old values and
    */
}

int main(int argc, char *argv[])
{

 int i;
 int nit = 0; /* number of iterations */

  
 if( argc != 2)
 {
   printf("Usage: gsref filename\n");
   exit(1);
 }
  
 /* Read the input file and fill the global data structure above */ 
 get_input(argv[1]);
 
 /* Check for convergence condition */
 /* This function will exit the program if the coffeicient will never converge to 
  * the needed absolute error. 
  * This is not expected to happen for this programming assignment.
  */
 check_matrix();
 
 nit = sequential();
 
 /* Writing to the stdout */
 /* Keep that same format */
 for( i = 0; i < num; i++)
   printf("%f\n",x[i]);
 
 printf("total number of iterations: %d\n", nit);
 
 exit(0);

}
