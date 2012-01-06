#include "syscall.h"
/*--------------------------------
exectest1.c

This is a simple nachos user progam to test
exec syscall
---------------------------------*/

int main()
{
	Write("\nEXEC-ING PROGRAM matmult.c and sort.c",38,1);
	Exec("../test/matmult",15);
	Exec("../test/sort",12);
	Exit(0);
}