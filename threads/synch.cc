// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!

// THE FOLLOWING CODE BELOW IS THE CODE ADDED BY ME ON 24-JAN-2009 
// NAME OF THIS CHANGE IS CHANGE#1

//----------------------------------------------------------------------
// Lock::Lock
// INTIALIZE A LOCK, SO THAT IT CAN BE USED BY THREADS FOR MUTUAL EXCLUSION
// DEBUG NAME IS THE ARBINTARY NAME FOR THIS LOCK THAT IS USED FOR DEBUGGING
//-------------------------------------------------------------------------
Lock::Lock(char* debugName)
{
	name=debugName;		// SETTING THE NAME FOR THE LOCK 
	lockOwner=NULL;		// INTIALLY THERE IS NO OWNER FOR THE LOCK 
	status=0;			// THE INTIAL STATUS OF THE LOCK IS FREE
	lockQueue=new List;	// CREATING A NEW LIST TO HOLD THE THREADS WAITING FOR THIS LOCK 
}
//-------------------------------------------------------------------------
//Lock::~Lock
// DEALLOCATE THIS LOCK WHEN THIS LOCK IS NOT NEEDED
//--------------------------------------------------------------------------
Lock::~Lock()
{

	delete lockQueue; // DELETING THE LIST SINCE THERE WOULD BE NO MORE LOCK 
}
//--------------------------------------------------------------------------
//Lock::Accquire
// THIS FUNCTION IS USED BY THE THREADS TO ACQUIRE A LOCK 
//---------------------------------------------------------------------------
void Lock::Acquire()
{
	IntStatus oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERUPPT SINCE THIS FUNCITON IS ATOMIC 
	if(currentThread==lockOwner)
	{
		// THE 'currentThread'  IS ALREADY OWNING THE LOCK 
		//printf("\n!!!! Nachos : This thread %s already owns the lock:  %s. !!!! \n ",currentThread->getName(),getName());
		(void)interrupt->SetLevel(oldLevel); // RESTORING INTERRUPTS
		
		return; // EXITING THE FUNCTION
	}
	else if(status==0) // CHECKING TO SEE IF THE LOCK IS FREE
	{
		// LOCK IS FREE
		//printf("\n ***The lock : %s is free \n",getName());
		//setStatus(1); // CHANGING THE STATE OF THE LOCK TO BUSY BY SETTING THE VALUE OF 'status' TO '1'
		status=1;
		//setOwner(currentThread); // CHANGING THE OWNERSHIP OF THE LOCK 
		lockOwner=currentThread;
		//printf("\n the owner of this lock : %s  is thread : %s",getName(),currentThread->getName());
	}
	else
	{		// LOCK IS BUSY 
		//printf("\n !!!! Nachos : The lock %s  is busy !!!! \n",getName());
		//printf("\n ***Adding thread: %s to this lock :%s's wait queue \n",currentThread->getName(),getName());
		lockQueue->Append((void *)currentThread); // ADDING THE CURRENT THREAD TO THIS LOCKS WAIT QUEUE
		//printf("\n ***Thread : %s Calling sleep()",currentThread->getName());
		currentThread->Sleep(); // CALLING SLEEP METHOD ON THE CURRENT THREAD TO GIVE UP THE CPU
	}
	(void)interrupt->SetLevel(oldLevel); // RESTORING INTERRUPTS
}
//--------------------------------------------------------------------------
//Lock::Release
// THIS FUNCTION IS USED BY THE THREADS TO RELEASE A LOCK 
//---------------------------------------------------------------------------
void Lock::Release() 
{
	Thread *nextOwner; // A THREAD POINTER THAT WILL HOLD THE REOMVED THREAD FROM THE WAIT QUEUE
	IntStatus oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERUPPT SINCE THIS FUNCITON IS ATOMIC 
	if(currentThread!=lockOwner) //CHECKING TO SEE IF THE CURRENT THREAD IS THE LOCK OWNER 
	{
		// THE CURRENT THREAD IS NOT THE OWNER 
		//printf("\n !!!! Nachos : This thread %s is not the owner of this lock.  !!!!\n",currentThread->getName());
		(void)interrupt->SetLevel(oldLevel); // RESTORING INTERRUPTS
		return;
	}
	else
	{
		//printf("\n thread: %s  is relasing the lock %s",currentThread->getName(),getName());
		// CURRENT THREAD IS THE OWNER
		if(lockQueue->IsEmpty())
		{
			// NO THREAD IS WAITING FOR THIS LOCK
			//printf("\n ***No thread is waiting for this lock : %s \n",getName());
			//setStatus(0); // MAKING THE LOCK FREE
			status=0;
			//setOwner(NULL); // CLEARING LOCK OWNERSHIP
			lockOwner=NULL;
			//printf("\n !!!! Nachos : lock %s released .  !!!!\n",name);
			(void)interrupt->SetLevel(oldLevel); // RESTORING INTREUPPTS
			return;
		}
		else
		{
			// WAKE-UP IMPLEMENTATION
			//printf("\n ***Trying to wake up a thread from the lock : %s's wait queue: \n",getName());
			nextOwner=(Thread *)lockQueue->Remove(); // REMOVING A WAITING THREAD FROM THE WAIT QUEUE
			//printf("\n ***The next thread that is awake now is :%s \n",nextOwner->getName());
			scheduler->ReadyToRun(nextOwner); // MAKE REMOVED THREAD RUNNABLE
			//setOwner(nextOwner); // CHANGING THE OWNERSHIP
			lockOwner=nextOwner;
		}
		(void) interrupt->SetLevel(oldLevel); //RESTORING INTRREUPT
	}

}

//----------------------------------------------------------------------
// Condition::Condition
// INTIALIZE A CONDITION VARIBALE, SO THAT IT CAN BE USED BY THREADS FOR SYNCRONISATION 
// AND SCEQUENCING DEBUG NAME IS THE ARBINTARY NAME FOR THIS LOCK THAT IS USED FOR DEBUGGING
//-------------------------------------------------------------------------
Condition::Condition(char* debugName) 
{
	name=debugName;				// SETTING A NAME FOR THE CONDITION VARIABLE
	savedLockPointer=NULL;		// INTIALLY THE SAVED LOCK POINTER IS NULL
	conditionQueue=new List;	// CREATING A NEW LIST FOR THIS CONDITION VARIABLE
}

//-------------------------------------------------------------------------
//Condition::~Condition
// DEALLOCATE THIS CONDITION VARIABLE WHEN THIS CONDITION VARIABLE IS NOT NEEDED
//--------------------------------------------------------------------------
Condition::~Condition() 
{
	delete conditionQueue;
	delete savedLockPointer;
}

//--------------------------------------------------------------------------
//Condition::Wait
//
//---------------------------------------------------------------------------

void Condition::Wait(Lock* conditionLock) 
{
	// ASSERT(FALSE); 
	IntStatus oldLevel=interrupt->SetLevel(IntOff);
	if(savedLockPointer==NULL)
	{
		savedLockPointer=conditionLock;
	}
	conditionLock->Release();
	//printf("\nthe thread %s is waiting on lock %s \n",currentThread->getName(),savedLockPointer->getName());
	conditionQueue->Append((void *)currentThread);
	currentThread->Sleep();
	conditionLock->Acquire();
	(void)interrupt->SetLevel(oldLevel);
}

void Condition::Signal(Lock* conditionLock)
{
	Thread *nextWaitingThread;
	IntStatus oldLevel=interrupt->SetLevel(IntOff);

	if(conditionQueue->IsEmpty())
	{
	   //printf("\nthread %s says no one to signal\n 1st condition empty",currentThread->getName());
		(void)interrupt->SetLevel(oldLevel);
		return;
	}
	else
	{
		if(conditionLock!=getsavedLock())
		{
			//printf("\n Sorry you have called signal on wrong lock \n");
			(void)interrupt->SetLevel(oldLevel);
			return;

		}
		else
		{
			//printf("\nthe thread %s is signaling on this lock %s\n",currentThread->getName(),savedLockPointer->getName());
			nextWaitingThread=(Thread *)conditionQueue->Remove();
			scheduler->ReadyToRun(nextWaitingThread); // MAKE REMOVED THREAD RUNNABLE
			if(conditionQueue->IsEmpty())
			{
				//printf("\nthread %s says no one to signal\n",currentThread->getName());
				saveLock(NULL);
			}

		}
	}
	(void)interrupt->SetLevel(oldLevel);
}

void Condition::Broadcast(Lock* conditionLock) 
{
	while(!(conditionQueue->IsEmpty()))
	{
		Signal(conditionLock);
	}
}
