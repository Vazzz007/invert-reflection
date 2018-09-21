#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <fenv.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "args.h"
#include "create_matrix.h"
#include "func_eval.h"

#define EPS 1e-16

/* factorial supports the following command-line arguments:
 * 
 * -i input_file_name.txt - name of the input file (default = NULL)
 * -n number - number of elements (default = 0)
 * -v - option for debugging
 * -f formula - define formula
 */

struct timespec diff(struct timespec start, struct timespec end);

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int main(int argc, char **argv){
    
    feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW);
    
    struct timespec time_start, time_end;
    
    int n = 0;
    int max_out = 0;
    char *inFileName = NULL;
    int verbose = 0;
    char *formula = NULL;
    double *X = NULL;
    double *A = NULL;
    FILE *fin;
    int result;

    
    
    if ((get_args(&n, &inFileName, &verbose, &formula, &max_out, argc, argv)) != 0){
        fprintf (stderr, "Error: Can't get arguments!\n");
        return -1;
    }
    
    if (inFileName == NULL){
        A = (double *)malloc (n * n * sizeof(double));
	X = (double *)malloc (n * n * sizeof(double));
	
        if ((A && X) != 1) {
            printf ("Error: Not enough memory for matrice A!\n");
	    if (A == NULL)
	      free(A);
	    if (X == NULL)
	      free(X);
            return -1;
        }
        
        if ((create_matrix(A, n, formula)) != 0){
	    printf ("Error: Can't create matrix!\n");
	    free(A);
	    free(X);
	    return -1;
	}
    } else {
	fin = fopen(inFileName, "r");
	
	if (fin == NULL){
	    printf("Error: Can't open file!\n");
	    return -1;
	}
	
	if (fscanf(fin, "%d", &n) != 1){
	    printf("Error: Can't read dimension from file!\n");
	    fclose(fin);
	    return -1;
	}
	
	if (n < 1){
	    printf ("Error: Invalid matrix dimension!\n");
	    fclose(fin);
	    return -1;
	}
	
	A = (double *)malloc (n * n * sizeof(double));
	X = (double *)malloc (n * n * sizeof(double));
	
        if ((A && X) != 1) {
            printf ("Error: Not enough memory for matrice A!\n");
	    if (A == NULL)
	      free(A);
	    if (X == NULL)
	      free(X);
	    fclose(fin);
            return -1;
        }
        
        if (InputMatrix(n, A, fin) != 0){
	    printf("Error: Can't read matrix from file!\n");
	    fclose(fin);
	    free(A);
	    free(X);
	    return -1;
	}
    }
    
    printf("\nMatrix A:\n");
    
    PrintMatrix(n, A, max_out);
    
    printf("\n");
    
    printf("Calculating...\n");
    
    if( clock_gettime( CLOCK_MONOTONIC, &time_start) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }
    
    result = InvertMatrix(n, A, X);
    
    if( clock_gettime( CLOCK_MONOTONIC, &time_end) == -1 ) {
        perror( "clock gettime" );
        exit( EXIT_FAILURE );
    }
    time_end = diff(time_start, time_end);
    
    if (result == -1){
        printf("Can't invert.\n");
    } else {
        printf("\nMatrix A^{-1}:\n");
        PrintMatrix(n, X, max_out);
        printf("\n");
        
        printf("Inversion time\t\t= %f sec.\n\n", 
               (double)time_end.tv_sec + (double)time_end.tv_nsec/(double)1000000000);
        
        
        if (inFileName == NULL){
            if ((create_matrix(A, n, formula)) != 0){
                printf ("Error: Can't create matrix!\n");
                free(A);
                free(X);
                return -1;
            }
        } else {
            fin = fopen(inFileName, "r");
	
            if (fin == NULL){
                printf("Error: Can't open file!\n");
                free(A);
                free(X);
                return -1;
            }
	
            if (fscanf(fin, "%d", &n) != 1){
                printf("Error: Can't read dimension from file!\n");
                free(A);
                free(X);
                fclose(fin);
                return -1;
            }

            if (n < 1){
                printf ("Error: Invalid matrix dimension!\n");
                free(A);
                free(X);
                fclose(fin);
                return -1;
            }
        
            if (InputMatrix(n, A, fin) != 0){
                printf("Error: Can't read matrix from file!\n");
                fclose(fin);
                free(A);
                free(X);
                return -1;
            }
            
            fclose(fin);
        }
        
        printf("Solution ||A * A^{-1} - I||\t= %e\n", SolutionError(n, A, X));
    }

    
    

    

    free(A);
    free(X);
    
    return 0;
}
