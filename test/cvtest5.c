/*
--------------------------------------------------
cvtest5.c

This is simple nachos user program that tries to access
the condition variable not created by it

--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int cv1=0;
	int l1=CreateLock();
	Write("\n Running cvtest5.c\n",22,1);
	Write("\nTESTING: CVTEST5.C TRIES TO ACCESS THE CV NOT CREATED BY IT",60,1);
	Exec("../test/cvtest1",15);
	Yield();
	Yield();
	Yield();
	WaitCondition(cv1,l1);
	SignalCondition(cv1,l1);
	BroadcastCondition(cv1,l1);
	Exit(0);
}