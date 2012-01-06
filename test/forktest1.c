/*
--------------------------------------------------
forktest1.c

This is a simple nachos user progam to test
fork system call;

--------------------------------------------------
*/

void fun1()
{
	Write("\n Inside fun1",13,1);
	Write("\n Exiting fun1",14,1);

	Exit(0);
}

int main()
{
	Write("\nTESTING FORK",13,1);
	Write("\n Inside main",13,1);
	Fork(fun1);
	Yield();
	Yield();
	Yield();
	Yield();
	Write("\n Exiting main",14,1);
	Exit(0);
}