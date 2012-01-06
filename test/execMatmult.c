#include "syscall.h"
/*--------------------------------
exectest1.c

This is a simple nachos user progam to test
exec syscall
---------------------------------*/

int main()
{
	Write("\nEXEC-ING PROGRAM matmult.c twice",33,1);
	Exec("../test/matmult",15);
	Exec("../test/matmult",15);
	Exit(0);
}