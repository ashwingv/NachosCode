/*
--------------------------------------------------
cvtest.c

This is simple nachos user program creates a condtion variable
and destroys the condition variable.
This program is written to check if create condition variable 
detroy condition variable system calls work properly and to check
if the destroyed condition variable reference is re-used

--------------------------------------------------
*/
#include "syscall.h"


int main()
{
	int cv1;
	int cv2;
	int cv3;
	Write("\n Running cvtest1.c\n",22,1);
	cv1=CreateCondition();
	Yield();
	cv2=CreateCondition();
	DestroyCondition(cv2);
	cv3=CreateCondition();
	Exit(0);
}