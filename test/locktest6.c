/*
--------------------------------------------------
locktest6.c

This is simple nachos user program creates a lock
and tries to destroy the lock when threads are waiting
int the lock queue

--------------------------------------------------
*/


int lock1;

void fun1()
{
	Write("\nInside fun1",12,1);
	AcquireLock(lock1);
}

void fun2()
{
	Write("\nInside fun2",12,1);
	AcquireLock(lock1);

}

int main()
{
	int lock1;

	Write("\n Running locktest1.c\n",22,1);
	Write("\nTRYING TO DESTROY A LOCK WHEN THREADS ARE WAITING ON THE LOCK QUEUE",68,1);
	lock1=CreateLock();
	AcquireLock(lock1);
	Fork(fun1);
	Fork(fun2);
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	DestroyLock(lock1);
	Exit(0);
}