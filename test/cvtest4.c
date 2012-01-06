/*
--------------------------------------------------
cvtest5.c

This is simple nachos user program to test the boundry 
to the creation of the condition variables

--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int cv1;
	unsigned int i=0;
	Write("\n Running locktest1.c\n",22,1);
	Write("\nTESTING THE BOUNDARY CONDTION ON LOCKS",39,1);
	for(i=0;i<67000;i++)
	{
		cv1=CreateCondition();
			if(cv1==-1)
			{
				break;
			}
	}
	Exit(0);
}