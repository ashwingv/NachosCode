/*
--------------------------------------------------
locktest3.c
This is a simple nachos user program which 
created a lock in and exec-s a locktest4 and the 
the newly created process tries to acquire the lock 
created by this process. We will get a Error Msg msg from
nahcos in locktest4 saying that locktest4 cannot 
access the lock variable created in locktest3
--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int lock1;
	int lock2;
	int lock3;
	Write("\n Running locktest3.c\n",22,1);
	Write("\nTESTING: LOCKTEST3.C EXEC-S LOCKTEST4.C AND LOXKTEST4.C TRIES TO ACQUIRE THE LOCK CREATED BY LOCKTEST3.C",105,1);
	lock1=CreateLock();
	Exec("../test/locktest4",17);
	lock2=CreateLock();
	DestroyLock(lock2);
	lock3=CreateLock();
	Exit(0);
}