/*
--------------------------------------------------
locktest4.c

This is a simple nachos user program. This is not
meant to be run alone. This progrma is exec-ed from
locktest3. This program tries to acquire release
and destroy the lock created by locktest3.So
nachos returns a error Msg saying that this process
cannot access the lock 
--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int lock4;
	int lock5=2;
	int lock1=1;
	Write("\n Running locktest4.c\n",22,1);
	Write("\nThis program should not be run alone it is writen to be exec-ed from locktest3",79,1);
	Write("\nTESTING: LOCKTEST4.C TRIES TO ACCESS THE CV NOT CREATED BY IT",62,1);
	lock4=CreateLock();
	AcquireLock(lock4);
	ReleaseLock(lock4);
	DestroyLock(lock4);

	AcquireLock(lock1); /* TRYING TO ACQUIRE A LOCK PASSING WHICH IS OUTSIDE THE ADDRESS SPACE OF THIS PROCESS */
	ReleaseLock(lock1); /* TRYING TO ACQUIRE A LOCK PASSING WHICH IS OUTSIDE THE ADDRESS SPACE OF THIS PROCESS */
	DestroyLock(lock1); /* TRYING TO ACQUIRE A LOCK PASSING WHICH IS OUTSIDE THE ADDRESS SPACE OF THIS PROCESS */
	Exit(0);
}