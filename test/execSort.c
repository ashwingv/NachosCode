#include "syscall.h"
/*--------------------------------
exectest1.c

This is a simple nachos user progam to test
exec syscall
---------------------------------*/

int main()
{
	Write("\nEXEC-ING PROGRAM sort.c twice",30,1);
	Exec("../test/sort",12);
	Exec("../test/sort",12);
	Exit(0);
}