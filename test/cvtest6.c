/*
--------------------------------------------------
cvtest6.c

This is simple nachos user program in which the main 
thread tries to destroy a condition variable on which 
few threads are waiting on.
--------------------------------------------------
*/

int cv1;
int l1;

void fun1()
{
	WaitCondition(cv1,l1);
	Exit(0);
}

int main()
{
	Write("\nTESTING: TRYING TO DESTROY A CONDITION VARUIABLE WHEN STILL THREADS ARE WAIITNG IN THE WAIT QUEUE",98,1);
	l1=CreateLock();
	cv1=CreateCondition();
	Fork(fun1);
	Yield();
	Yield();
	Yield();
	DestroyCondition(cv1);
	Exit(0);
}