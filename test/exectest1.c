#include "syscall.h"
/*--------------------------------
exectest1.c

This is a simple nachos user progam to test
exec syscall
---------------------------------*/

int main()
{
	Write("\nTESTING EXEC",13,1);
	Write("\nEXEC-ING PROGRAM LOCKTEST1.C",29,1);
	Exec("../test/locktest1",17);
	Exit(0);
}
