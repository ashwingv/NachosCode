#include "syscall.h"
/*
--------------------------------------------------
forktest2.c

This is a simple nachos user progam to test
fork system call to the limts on the number of threads
for each process

--------------------------------------------------
*/
int c1;
int l1;
void fun2()
{
	Write("\nInside Fun2",12,1);
	l1=CreateLock();
	c1=CreateCondition();
	WaitCondition(c1,l1);
	Exit(0);
}

int main()
{
	Write("\nTESTING THE LIMITS OF FORK",27,1);
	while(1)
	{
		Fork(fun2);
		Yield();
		Yield();
		Yield();
		Yield();
	}
	Exit(0);
}