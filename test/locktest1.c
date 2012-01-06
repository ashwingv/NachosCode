/*
--------------------------------------------------
locktest1.c

This is simple nachos user program creates a lock
and destroys the lock and creates a new lock and 
this program is written to check if create lock 
detroy lock system calls work properly and to check
if the destroyed lock reference is re-used

--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int lock1;
	int lock2;
	int lock3;
	Write("\n Running locktest1.c\n",22,1);
	lock1=CreateLock();
	lock2=CreateLock();
	DestroyLock(lock2);
	lock3=CreateLock();
	Exit(0);
}