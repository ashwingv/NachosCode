/*
--------------------------------------------------
locktest5.c

This is simple nachos user program to test the boundry 
to the creation of the locks
--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int lock1;
	unsigned int i=0;
	Write("\n Running locktest1.c\n",22,1);
	for(i=0;i<67000;i++)
	{
		lock1=CreateLock();
			if(lock1==-1)
			{
				break;
			}
	}
	Exit(0);
}