/*
--------------------------------------------------
locktest2.c

This is simple nachos user program to test the 
acquire lock and release lock syscalls. This 
also test the Acquire Relase and Destory functions
on a lock that is not yet created.

This also test invalid input to the syscalls like 
giving in a -ve number or give the number that 
exceeds the max lock counts

It also ties to call Acquire Release and Destory 
without passing any reference.In such a case
Nachos takes a garbage value was left behind in 
regeisters and works  on them.

--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int lock4;
	int lock5=2;
	int lock1=1;
	Write("\n Running locktest2.c\n",22,1);
	lock4=CreateLock();

	AcquireLock(); /* TRYING TO ACQUIRE A LOCK WITH OUT PASSING ANY REFERENCE */
	ReleaseLock(); /* TRYING TO RELEASE A LOCK WITH OUT PASSING ANY REFERENCE */
	DestroyLock(); /* TRYING TO DESTROY A LOCK WITH OUT PASSING ANY REFERENCE */

	AcquireLock(lock5); /* TRYING TO ACQUIRE A LOCK THAT IS NOT YET CREATED */
	ReleaseLock(lock5);	/* TRYING TO RELEASE A LOCK THAT IS NOT YET CREATED */
	DestroyLock(lock5); /* TRYING TO DESTROY A LOCK THAT IS NOT YET CREATED */

	AcquireLock(-1); /* TRYING TO ACQUIRE A LOCK PASSING A INVALID REFERENCE */
	ReleaseLock(-1); /* TRYING TO RELEASE A LOCK PASSING A INVALID REFERENCE */
	DestroyLock(-1); /* TRYING TO DESTROY A LOCK PASSING A INVALID REFERENCE */

	AcquireLock(70000); /* TRYING TO ACQUIRE A LOCK PASSING A INVALID REFERENCE */
	ReleaseLock(70000); /* TRYING TO RELEASE A LOCK PASSING A INVALID REFERENCE */
	DestroyLock(70000); /* TRYING TO DESTROY A LOCK PASSING A INVALID REFERENCE */

	Exit(0);
}