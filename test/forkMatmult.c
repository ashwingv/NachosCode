/* matmult.c 
 *    Test program to do matrix multiplication on large arrays.
 *
 *    Intended to stress virtual memory system.
 *
 *    Ideally, we could read the matrices off of the file system,
 *	and store the result back to the file system!
 */

#include "syscall.h"

#define Dim 	20	/* sum total of the arrays doesn't fit in 
			 * physical memory 
			 */

int A[Dim][Dim];
int B[Dim][Dim];
int C[Dim][Dim];

int A1[Dim][Dim];
int B1[Dim][Dim];
int C1[Dim][Dim];

void matmult()
{
    int i, j, k;
	/*int Dim=20;*/




    for (i = 0; i < Dim; i++)		/* first initialize the matrices */
	for (j = 0; j < Dim; j++) {
	     A[i][j] = i;
	     B[i][j] = j;
	     C[i][j] = 0;
	}

    for (i = 0; i < Dim; i++)		/* then multiply them together */
	for (j = 0; j < Dim; j++)
            for (k = 0; k < Dim; k++)
		 C[i][j] += A[i][k] * B[k][j];

    Exit(C[Dim-1][Dim-1]);		/* and then we're done */
}

void matmult1()
{
    int i, j, k;
	/*int Dim=20;*/




    for (i = 0; i < Dim; i++)		/* first initialize the matrices */
	for (j = 0; j < Dim; j++) {
	     A1[i][j] = i;
	     B1[i][j] = j;
	     C1[i][j] = 0;
	}

    for (i = 0; i < Dim; i++)		/* then multiply them together */
	for (j = 0; j < Dim; j++)
            for (k = 0; k < Dim; k++)
		 C1[i][j] += A1[i][k] * B1[k][j];

    Exit(C1[Dim-1][Dim-1]);		/* and then we're done */
}



int main()
{

	Write("\nFroking PROGRAM matmult.c twice",32,1);
	Fork(matmult);
	Fork(matmult1);
	Exit(0);

}