// threadtest.cc
//    Simple test case for the threads assignment.
//
//    Create two threads, and have them context switch
//    back and forth between themselves by calling Thread::Yield,
//    to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

//#include "copyright.h"
//#include "system.h"

#include "copyright.h"
#include "system.h"
#include "string.h"
//#include <string>
//#include <ctype>
#ifdef CHANGED
#include "synch.h"
#endif

#ifdef CHANGED

//----------------------------------------------------------------------
// SimpleThread
//     Loop 5 times, yielding the CPU to another ready thread
//    each iteration.
//
//    "which" is simply a number identifying the thread, for debugging
//    purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
   
    for (num = 0; num < 5; num++) {
    printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest
//     Set up a ping-pong between two threads, by forking a thread
//    to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

//void
//ThreadTest()
//{
    //DEBUG('t', "Entering SimpleTest");

   // Thread *t = new Thread("forked thread");

    //t->Fork(SimpleThread, 1);
    //SimpleThread(0);
    // --------------------------------------------------
// Test Suite
// --------------------------------------------------


// --------------------------------------------------
// Test 1 - see TestSuite() for details
// --------------------------------------------------
Semaphore t1_s1("t1_s1",0);       // To make sure t1_t1 acquires the
                                  // lock before t1_t2
Semaphore t1_s2("t1_s2",0);       // To make sure t1_t2 Is waiting on the
                                  // lock before t1_t3 releases it
Semaphore t1_s3("t1_s3",0);       // To make sure t1_t1 does not release the
                                  // lock before t1_t3 tries to acquire it
Semaphore t1_done("t1_done",0);   // So that TestSuite knows when Test 1 is
                                  // done
Lock t1_l1("t1_l1");          // the lock tested in Test 1

// --------------------------------------------------
// t1_t1() -- test1 thread 1
//     This is the rightful lock owner
// --------------------------------------------------
void t1_t1() {
    t1_l1.Acquire();
    t1_s1.V();  // Allow t1_t2 to try to Acquire Lock
 
    printf ("%s: Acquired Lock %s, waiting for t3\n",currentThread->getName(),
        t1_l1.getName());
    t1_s3.P();
    printf ("%s: working in CS\n",currentThread->getName());
    for (int i = 0; i < 1000000; i++) ;
    printf ("%s: Releasing Lock %s\n",currentThread->getName(),
        t1_l1.getName());
    t1_l1.Release();
    t1_done.V();
}

// --------------------------------------------------
// t1_t2() -- test1 thread 2
//     This thread will wait on the held lock.
// --------------------------------------------------
void t1_t2() {

    t1_s1.P();    // Wait until t1 has the lock
    t1_s2.V();  // Let t3 try to acquire the lock

    printf("%s: trying to acquire lock %s\n",currentThread->getName(),
        t1_l1.getName());
    t1_l1.Acquire();

    printf ("%s: Acquired Lock %s, working in CS\n",currentThread->getName(),
        t1_l1.getName());
    for (int i = 0; i < 10; i++)
    ;
    printf ("%s: Releasing Lock %s\n",currentThread->getName(),
        t1_l1.getName());
    t1_l1.Release();
    t1_done.V();
}

// --------------------------------------------------
// t1_t3() -- test1 thread 3
//     This thread will try to release the lock illegally
// --------------------------------------------------
void t1_t3() {

    t1_s2.P();    // Wait until t2 is ready to try to acquire the lock

    t1_s3.V();    // Let t1 do it's stuff
    for ( int i = 0; i < 3; i++ ) {
    printf("%s: Trying to release Lock %s\n",currentThread->getName(),
           t1_l1.getName());
    t1_l1.Release();
    }
}

// --------------------------------------------------
// Test 2 - see TestSuite() for details
// --------------------------------------------------
Lock t2_l1("t2_l1");        // For mutual exclusion
Condition t2_c1("t2_c1");    // The condition variable to test
Semaphore t2_s1("t2_s1",0);    // To ensure the Signal comes before the wait
Semaphore t2_done("t2_done",0);     // So that TestSuite knows when Test 2 is
                                  // done

// --------------------------------------------------
// t2_t1() -- test 2 thread 1
//     This thread will signal a variable with nothing waiting
// --------------------------------------------------
void t2_t1() {
    t2_l1.Acquire();
    printf("%s: Lock %s acquired, signalling %s\n",currentThread->getName(),
       t2_l1.getName(), t2_c1.getName());
    t2_c1.Signal(&t2_l1);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
       t2_l1.getName());
    t2_l1.Release();
    t2_s1.V();    // release t2_t2
    t2_done.V();
}

// --------------------------------------------------
// t2_t2() -- test 2 thread 2
//     This thread will wait on a pre-signalled variable
// --------------------------------------------------
void t2_t2() {
    t2_s1.P();    // Wait for t2_t1 to be done with the lock
    t2_l1.Acquire();
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
       t2_l1.getName(), t2_c1.getName());
    t2_c1.Wait(&t2_l1);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
       t2_l1.getName());
    t2_l1.Release();
}
// --------------------------------------------------
// Test 3 - see TestSuite() for details
// --------------------------------------------------
Lock t3_l1("t3_l1");        // For mutual exclusion
Condition t3_c1("t3_c1");    // The condition variable to test
Semaphore t3_s1("t3_s1",0);    // To ensure the Signal comes before the wait
Semaphore t3_done("t3_done",0); // So that TestSuite knows when Test 3 is
                                // done

// --------------------------------------------------
// t3_waiter()
//     These threads will wait on the t3_c1 condition variable.  Only
//     one t3_waiter will be released
// --------------------------------------------------
void t3_waiter() {
    t3_l1.Acquire();
    t3_s1.V();        // Let the signaller know we're ready to wait
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
       t3_l1.getName(), t3_c1.getName());
    t3_c1.Wait(&t3_l1);
    printf("%s: freed from %s\n",currentThread->getName(), t3_c1.getName());
    t3_l1.Release();
    t3_done.V();
}


// --------------------------------------------------
// t3_signaller()
//     This threads will signal the t3_c1 condition variable.  Only
//     one t3_signaller will be released
// --------------------------------------------------
void t3_signaller() {

    // Don't signal until someone's waiting
   
    for ( int i = 0; i < 5 ; i++ )
    t3_s1.P();
    t3_l1.Acquire();
    printf("%s: Lock %s acquired, signalling %s\n",currentThread->getName(),
       t3_l1.getName(), t3_c1.getName());
    t3_c1.Signal(&t3_l1);
    printf("%s: Releasing %s\n",currentThread->getName(), t3_l1.getName());
    t3_l1.Release();
    t3_done.V();
}
 
// --------------------------------------------------
// Test 4 - see TestSuite() for details
// --------------------------------------------------
Lock t4_l1("t4_l1");        // For mutual exclusion
Condition t4_c1("t4_c1");    // The condition variable to test
Semaphore t4_s1("t4_s1",0);    // To ensure the Signal comes before the wait
Semaphore t4_done("t4_done",0); // So that TestSuite knows when Test 4 is
                                // done

// --------------------------------------------------
// t4_waiter()
//     These threads will wait on the t4_c1 condition variable.  All
//     t4_waiters will be released
// --------------------------------------------------
void t4_waiter() {
    t4_l1.Acquire();
    t4_s1.V();        // Let the signaller know we're ready to wait
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
       t4_l1.getName(), t4_c1.getName());
    t4_c1.Wait(&t4_l1);
    printf("%s: freed from %s\n",currentThread->getName(), t4_c1.getName());
    t4_l1.Release();
    t4_done.V();
}


// --------------------------------------------------
// t2_signaller()
//     This thread will broadcast to the t4_c1 condition variable.
//     All t4_waiters will be released
// --------------------------------------------------
void t4_signaller() {

    // Don't broadcast until someone's waiting
   
    for ( int i = 0; i < 5 ; i++ )
    t4_s1.P();
    t4_l1.Acquire();
    printf("%s: Lock %s acquired, broadcasting %s\n",currentThread->getName(),
       t4_l1.getName(), t4_c1.getName());
    t4_c1.Broadcast(&t4_l1);
    printf("%s: Releasing %s\n",currentThread->getName(), t4_l1.getName());
    t4_l1.Release();
    t4_done.V();
}
// --------------------------------------------------
// Test 5 - see TestSuite() for details
// --------------------------------------------------
Lock t5_l1("t5_l1");        // For mutual exclusion
Lock t5_l2("t5_l2");        // Second lock for the bad behavior
Condition t5_c1("t5_c1");    // The condition variable to test
Semaphore t5_s1("t5_s1",0);    // To make sure t5_t2 acquires the lock after
                                // t5_t1

// --------------------------------------------------
// t5_t1() -- test 5 thread 1
//     This thread will wait on a condition under t5_l1
// --------------------------------------------------
void t5_t1() {
    t5_l1.Acquire();
    t5_s1.V();    // release t5_t2
    printf("%s: Lock %s acquired, waiting on %s\n",currentThread->getName(),
       t5_l1.getName(), t5_c1.getName());
    t5_c1.Wait(&t5_l1);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
       t5_l1.getName());
    t5_l1.Release();
}

// --------------------------------------------------
// t5_t1() -- test 5 thread 1
//     This thread will wait on a t5_c1 condition under t5_l2, which is
//     a Fatal error
// --------------------------------------------------
void t5_t2() {
    t5_s1.P();    // Wait for t5_t1 to get into the monitor
    t5_l1.Acquire();
    t5_l2.Acquire();
    printf("%s: Lock %s acquired, signalling %s\n",currentThread->getName(),
       t5_l2.getName(), t5_c1.getName());
    t5_c1.Signal(&t5_l2);
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
       t5_l2.getName());
    t5_l2.Release();
    printf("%s: Releasing Lock %s\n",currentThread->getName(),
       t5_l1.getName());
    t5_l1.Release();
}

// --------------------------------------------------
// TestSuite()
//     This is the main thread of the test suite.  It runs the
//     following tests:
//
//       1.  Show that a thread trying to release a lock it does not
//       hold does not work
//
//       2.  Show that Signals are not stored -- a Signal with no
//       thread waiting is ignored
//
//       3.  Show that Signal only wakes 1 thread
//
//     4.  Show that Broadcast wakes all waiting threads
//
//       5.  Show that Signalling a thread waiting under one lock
//       while holding another is a Fatal error
//
//     Fatal errors terminate the thread in question.
// --------------------------------------------------
void TestSuite() {
    Thread *t;
    char *name;
    int i;
   
    // Test 1

    printf("Starting Test 1\n");

    t = new Thread("t1_t1");
    t->Fork((VoidFunctionPtr)t1_t1,0);

    t = new Thread("t1_t2");
    t->Fork((VoidFunctionPtr)t1_t2,0);

    t = new Thread("t1_t3");
    t->Fork((VoidFunctionPtr)t1_t3,0);

    // Wait for Test 1 to complete
    for (  i = 0; i < 2; i++ )
    t1_done.P();

    // Test 2

    printf("Starting Test 2.  Note that it is an error if thread t2_t2\n");
    printf("completes\n");

    t = new Thread("t2_t1");
    t->Fork((VoidFunctionPtr)t2_t1,0);

    t = new Thread("t2_t2");
    t->Fork((VoidFunctionPtr)t2_t2,0);

    // Wait for Test 2 to complete
    t2_done.P();

    // Test 3

    printf("Starting Test 3\n");

    for (  i = 0 ; i < 5 ; i++ ) {
    name = new char [20];
    sprintf(name,"t3_waiter%d",i);
    t = new Thread(name);
    t->Fork((VoidFunctionPtr)t3_waiter,0);
    }
    t = new Thread("t3_signaller");
    t->Fork((VoidFunctionPtr)t3_signaller,0);

    // Wait for Test 3 to complete
    for (  i = 0; i < 2; i++ )
    t3_done.P();

    // Test 4

    printf("Starting Test 4\n");

    for (  i = 0 ; i < 5 ; i++ ) {
    name = new char [20];
    sprintf(name,"t4_waiter%d",i);
    t = new Thread(name);
    t->Fork((VoidFunctionPtr)t4_waiter,0);
    }
    t = new Thread("t4_signaller");
    t->Fork((VoidFunctionPtr)t4_signaller,0);

    // Wait for Test 4 to complete
    for (  i = 0; i < 6; i++ )
    t4_done.P();

    // Test 5

    printf("Starting Test 5.  Note that it is an error if thread t5_t1\n");
    printf("completes\n");

    t = new Thread("t5_t1");
    t->Fork((VoidFunctionPtr)t5_t1,0);

    t = new Thread("t5_t2");
    t->Fork((VoidFunctionPtr)t5_t2,0);

}
#endif



//----------------------------------------------------------------------------------------------------------
// PROBLEM 2
//-----------------------------------------------------------------------------------------------------------

															
int grandTotalOfVisitors=0;					// KEEPS TRACK OF THE GRAND TOTAL OF VISITOR IN THE SIMULATION THIS DOES NTO INCLUDE THE DRIVERS COUNT
int countOfExitedCars=0;					// COUNT OF THE CARS THAT EXITED
Lock *countOfExitedCarLock;					// LOCK TO THE ABOVE SHARED VARIABLE
int countOfParkedCars=0;					// COUNT OF THE CARS THAT ARE PARKED

//-------------------------------------------------------------------------------------- GLOBAL VARIABLES FOR VALETS---------------------------------------------------------------------------------------------

int totalNumberOfValets;					// INTEGER VARIABLE TO HOLD THE TOTAL NUMBER OF VALETS IN SIMULATION

Condition **CV_waitingForValetManager;

Condition **CV_waitingInBackRoom;

int valetManagerWaiting=0;
Lock *valetManagerWaitingLock;
Condition *CV_valetManagerWaiting;


int *valetStatus;							// THIS IS THE ARRAY THAT HOLDS THE STATUS OF  EACH VALETS
                                               // '-5' - VALET IN BENCH
                                               // '-1' - VALET IS FREE 
											   // '0-N' - ID OF THE CAR HE IS GOIN TO SERVICE
Lock **valetStatusLock;						// THIS IS THE ARRAY OF LOCKS TO EACH OF THE VALETS STATUS
Condition **CV_valetWaitingForDriver;		// THIS IS A ARRAY OF CONDITION VARIBALE THAT THE VALETS USE TO WAIT FOR A DRIVER TO HAND OVER THE KEYS

int countOfWaitingValets=0;					// VARIABLE TO KEEP TRACK OF THE VALETS WHO ARE WAITING ON THE BENCH
											//'0' - NO VALETS IN BENCH 'N' -N VALETS ARE IN BENCH
Lock *waitingValetCountLock;				// A LOCK TO CONTROL ACCES TO THE SAHRED VARIBALE COUNT OF WAITING VALETS
Condition *CV_countOfWaitingValets;			// CONDITION VARIABLE ON WHICH THE VALETS WAITS ON THE BENCH

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- GLOBAL VARIABLES FOR CARS AND	LIMOUSINES AND LINMOUSINE AND CAR DRIVER -----------------------------------------------------------------------------

int totalNumberOfCars;						// A INTEGER TO REPRESENT THE TOTAL NUMBER OF CARS IN THE SIMUALTION
int totalNumberOfDrivers;					// A INTEGER TO REPRESENT THE TOTAL NUMBER OF CAR DRIVERS IN SIMULATION ALWAYS EQUAL TO THE TOTAL NUMBER OF CARS
int *visitor;								// A INTEGER TO HOLD THE ARRAY OF SIZE GRAND TOTAL NUMBER OF VISITORS THAT REPRESENT THEIR ASSOCIATION WITH A CAR

int totalCountOfLimos;						// A INTERGER REPRESENTING THE TOTAL NUMBER OF LIMOUSINES.
int totalCountOfLimoDrivers;				// A INTERGER REPRESENTING THE TOTAL NUMBER OF LIMOUSINE DRIVERS ALWAYS EQUAL TO THE TOTAL LIMOUSINE COUNT

int limoQueueLength=0;						// VARIABLE TO KEEP TRACK OF THE LIMOUSINES QUEUE LENGTH
Lock *limoQueueLengthLock;					// A LOCK  TO CONTROL THE ACCESS TO THE SHARED VARIABLE
Condition *CV_limosWaitingToGetParked;		//	CONDITION VARIABLE ON WHICH THE LIMMOUSINE DRIVERS WAIT TO GET PARKED

int carQueueLength=0;						// VARIABLE TO KEEP TRACK OF THE CAR QUEUE LENGTH
                      // '0'- NO CARS IN QUEUE, 'N'- N CARS ARE IN QUEUE
Lock *carQueueLengthLock;					// A LOCK FOR THE ABOVE SHARED VARIABLE
Condition *CV_waitingToGetParked;			// CONDITION VARIABLE ON WHICH THE DRIVERS(CARS) WAIT TO GET PARKED

int exitLimoQueueLength=0;
Lock *exitLimoQueueLengthLock;
Condition *CV_exitLimoQueueLength;

int exitCarQueueLength=0;
Lock *exitCarQueueLengthLock;
Condition *CV_exitCarQueueLength;

int *readyToPark;							// ARRAY TO HOLD THE PARKING STATUS OF EACH CAR; '0'- CAR BIT READY TO GET PARKED '1'- CAR READY TO GET PARKED
Lock **readyToParkLock;						// ARRAY OF LOCKS TO CONTROL ACCESS TO THE ABOVE SHARED VARIABLE
Condition **CV_waitingToParkCar	;			// CONDITION VARIABLE ON WHICH THE VALET WAITS TO PARK A CAR

int *visitorPleaseGetOut;					// ARRAY TO HOLD THE INDICATOR TO VISITORS TO GET OUT OF EACH CAR; '0'- STAY IN CAR '1'- PLEASE GET OUT
Lock **visitorPleaseGetOutLock;				// LOCK TO CONTROL ACCESS TO THE SHARED VARIABLE
Condition **CV_visitorPleaseGetOut;			// CONSDITON VARIABLE ON WHICH THE VISITORS WAITS TO GET OF THE CAR

int *visitorPleaseGetIn;
Lock **visitorPleaseGetInLock;
Condition **CV_visitorPleaseGetIn;

int *totalNumberOfVisitors;					// ARRAY IDICATING THE TOTAL NUMBER OF VISITORS IN EACH CAR
Lock **totalNumberOfVisitorsLock;			// LOCK TO CONTROL THE ACCESS TO THE SHARED VARIABLE

int *visitorCount;							// ARRAY THAT HOLDS THE CURRENT COUNT OF VISITORS IN EACH CAR '0'- N VISITOR IN CAR
Lock **visitorCountLock;					// LOCK TO CONTROL ACCESS TO THE SHARED VARIABLE
Condition **CV_visitorCount;				// CONDITION VARIABLE ON WHICH THE DRIVER WAITS FOR THE VISITOR TO GET OUT

int *carParked;								// ARRAY OF INDIACTORS THAT SAY WHETHER A CAR IS PARKED OR NOT '0'- CAR NOT PARKED '1' - CAR PARKED
Lock **carParkedLock;						// LOCK TO CONTORL ACCESS TO THE SHARED VARIBALE

int *parkingToken;							// ARRAY TO KEEP NOTE OF THE PARKING TOKEN FOR EACH CAR MOSTLY THE CAR NUMBER ITSELF
Lock **parkingTokenLock;					// LOCK TO CONTROL ACCESS TO THE SHARED VARIABLE
Condition **CV_parkingToken;				// CONDITION VAIABLE ON WHICH THE DRIVER WAITS TO RECEIVE THE PARKING TOKEN FOR HIS CAR

int *valetReadyWithCar;						// ARRAY TO KEEP TRACK OF THE WHETHER THE VALET IS READY WITH THE CAR
Lock **valetReadyWithCarLock;				// LOCK FOR THE ABOVE SHARED VARIABLE
Condition **CV_valetReadyWithCar;			// CONDITION VARIABLE ON WHICH THE VALETS SINGAL DRIVER AND DRIVER WAITS

int *visitorsReadyToGoHome;					// ARRAY TO KEEP THE TRACK OF THE NUMBER OF VISITORS BACK FROM THE MUSEUM IF THIS COUNT IS EQUAL TO THE TOTAL NUMBER OF VISITORS IN CAR
									// THE LAST VISITOR SIGNALS THE DRIVER THAT THEY ARE READY TO LEAVE
Lock **visitorsReadyToGoHomeLock;			// LOCK FOR THE ABOVE SHARED VARIABLE
Condition **CV_visitorReadyToGoHome;		// CONDITION VARIABLE ON WHICH THE THE VISITORS WAIT FOR THE DRIVER

int *driverStatus;							// ARRAY INIDCATING THE DRIVER STATUS IF '0' DRIVER IS NOT READY TO TAKE THEM BACK IF '1' DRIVER IS READY TO TAKE THEM BACK 
Lock **driverStatusLock;					// LOCK FOR THE ABOVE SHARED VARIABLE
Condition **CV_driverStatus;				// CONDITION VARIABLE ON WHICH THE DRIVER WAITS FOR THE VISITORS TO ARRIVE

int carDriversWaitingToExit;
Lock *carDriverWaitingToExitLock;
Condition *CV_carDriverWaitingToExit;

int limoDriverWaitingToExit;
Lock *limoDriverWaitingToExitLock;
Condition *CV_limoDriverWaitingToExit;

int *tipsForThisCar;						// ARRAY THAT TELL THE VALET THE TIP AMOUNT
Lock **tipsForThisCarLock;						// LOCK TO THE ABOVE SAHRED VARIABLE		
Condition **CV_tipsForThisCar;				// CONDITION ON WHICH THE VALETS WAIT TO GET THEIR TIP

int *carKeysReady;							// ARRAY THAT INDICATES THAT THE CAR KEY IS READY TO BE TAKEN BY THE DRIVER
Lock **carKeysReadyLock;					// LOCK TO THE ABOVE SHARED VARIABLE
Condition **CV_carKeysReady;				// CONDITON VARIABLE ON WHICH THE DRIVERS WAIT TO GET THEIR KESY BACK

int *carReadyToGo;							// ARRAY THAT HOLD THE IDICATION THAT TELL THE DRIVER THE CAR IS READY TO GO 
Lock **carReadyToGoLock;					// LOCK FOR THE ABOVE SHARED VARIABLE
Condition **CV_carReadyToGo;

Condition **visitorsPleaseGetInCar;			// CONDITION ON WHICH THE DRIVERS WAIT FOR HIS VISITORS TO GET IN THE CAR


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------ GLOBAL VARIABLES FOR TICKET TAKERS----------------------------------------------------------------------------------------

int totalNumberOfTicketTakers;					// INTEGER VARIABLE TO HOLD THE TOTAL NUMBER OF TICKET TAKERS IN SIMULATION
				
int totalNumberOfActiveTicketTakers=0;			// INTEGER VARIABLE THAT HOLD THE COUNT OF ACTIVE TICKET TAKERS
Lock *totalNumberOfActiveTicketTakersLock;		// LOCK TO THE ABOVE SHARED COUNT
Condition *CV_noTicketTakers;					// CONDITION VARIABLE ON WHICH THE VISITORS WAIT IF THERE ARE NO ACTIVE TICKET TAKERS

Lock *ticketTakerLock;
Condition *CV_ticketTaker;

int *ticketTakerQueueLength;					// THIS IS THE ARRAY OF SIZE = TO THE TOTAL NUMBER OF TICKET TAKERS THAT TELLS THE LENGTH OF VISITORS IN HIS QUEUE - SHARED VARIABLE
Lock **ticketTakerQueueLengthLock;				// THIS IS ARRAY OF LOCKS TO THE ABOVE SHARED VARIABLE;
Condition **CV_ticketTakerQueueLength;			// THIS IS THE ARRAY OF CONDITION VARIABLE ON WHICH THE VISITORS WAIT IN QUEUE TO GET ACCEPTED BY A TICKET TAKER
Condition **CV_waitingTicketTaker;				// THIS IS THE ARRAY OF CONDITION VARIABLE ON WHICH THE TICKET TAKERS WAIT IF THERE IS NO ONE IN THE QUEUE

int totalNumberOfVisitorsAccepted=0;			// AN INTEGER TO KEEP TRACK OF THE TOTAL NUMBER OF VISITORS ACCPETED 
Lock *totalNumberOfVisitorsAcceptedLock;		// A LOCK TO THE ABOVE SHARED VARIABLE

int *visitorMuseumEntryStatus;					// THIS IS ARRAY THAT INDICATES THE VISITOR WETHER HE CAN GET INTO THE MUSEUM OR NOT
                                                // IF IT IS '0' VISITOR CANNOT GET IN OR IF IT IS '1' THE VISITOR CAN GET IT
                                                // THIS IS CHANGED BY THE TICKET TAKER
Lock **visitorMuseumEntryStatusLock;			// LOCK TO THE ABOVE SHARED VARIBALE
Condition **CV_visitorMuseumEntryStatus;		// CONDITION VARIABLE ON WHICH THE VISITORS WAIT TO ENTER THE MUSEUM

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
// THIS IS THE VALET MANAGER FUNCTION 
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void valetManager(int i)
{
	int id=i;

	while(1)
	{
		//printf("\n VM: the count of excited cars is %d",countOfExitedCars);
		//printf("\n VM: the count of total number of cars is %d",totalNumberOfCars);
		//printf("\n VM: the count of total number of limosine is %d",totalCountOfLimos);
		//printf("\n VM: the count of valets in back room is %d",(totalNumberOfValets-countOfWaitingValets));
		//printf("\n the valet Manager comes in ");
		//---------------------------- SHOULD SEND ONE TO THE BACK ROOM --------------------------
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();
		currentThread->Yield();

		waitingValetCountLock->Acquire();
		if(countOfWaitingValets>=2)
		{
			valetManagerWaitingLock->Acquire();
			valetManagerWaiting++;
			printf("\n Valet Manager is signaling a valet ");
			CV_countOfWaitingValets->Signal(waitingValetCountLock);
			waitingValetCountLock->Release();
			printf(" \n Valet Manager is waiting for the valet to come to him ");
			CV_valetManagerWaiting->Wait(valetManagerWaitingLock);
			valetManagerWaitingLock->Release();
			for(int j=0;j<totalNumberOfValets;j++)
			{
				valetStatusLock[j]->Acquire();
				if(valetStatus[j]==-9)
				{
					valetStatus[j]=-10;
					printf("\n Valet Manager has asked valet %d to go to the back room ",j);
					CV_waitingForValetManager[j]->Signal(valetStatusLock[j]);
					valetStatusLock[j]->Release();
					break;
				}
				else
				{
					valetStatusLock[j]->Release();
				}
			}
		}
		else
		{
					waitingValetCountLock->Release();
		}
		//-------- SHOULD WAKE ONE UP FROM THE BACK ROOM----------------
		limoQueueLengthLock->Acquire();
		carQueueLengthLock->Acquire();
		if((limoQueueLength+carQueueLength)>=4)
		{
			limoQueueLengthLock->Release();
			carQueueLengthLock->Release();
			for(int j=0;j<totalNumberOfValets;j++)
			{
				valetStatusLock[j]->Acquire();
				if(valetStatus[j]==-10)
				{
					valetStatus[j]=-5;
					CV_waitingInBackRoom[j]->Signal(valetStatusLock[j]);
					valetStatusLock[j]->Release();
					break;
				}
				else
				{
					valetStatusLock[j]->Release();
				}
			}
		}
		else
		{
			limoQueueLengthLock->Release();
			carQueueLengthLock->Release();
		}
	}

}

//-----------------------------------------------------------------------------------------------------------------------------------------------
// THIS IS THE VALET FUNCITON WHICH DOES THE WORK OF THE VALET.
// THIS IS FUNCITON TAKES AN INTEGER ARGUMENT WHICH IS THE IDENTIFIER OF THE VALET
//------------------------------------------------------------------------------------------------------------------------------------------------
void valet(int i)
{
    int id=i; // RECORDS HIS ID
    int carToService=-1; // INITALLY THE CAR TO SERVICE IS -1
	int token=-1;
	while(1)
	{
					valetStatusLock[id]->Acquire(); // ADDED TO AID INTERACTION FOR RETURNING CARS
					valetStatus[id]=-5;				// ADDED TO AID INTERACTION FOR RETURNING CARS
					valetStatusLock[id]->Release();	// ADDED TO AID INTERACTION FOR RETURNING CARS
					waitingValetCountLock->Acquire();
					countOfWaitingValets++;
					//valetManagerWaitingLock->Release();
					printf("\n ++ The count of waiting valet in bench is %d \n",countOfWaitingValets);
					CV_countOfWaitingValets->Wait(waitingValetCountLock);
					countOfWaitingValets--;
					printf("\n -- The count of waiting valet in bench is %d \n",countOfWaitingValets);
					waitingValetCountLock->Release();
					valetManagerWaitingLock->Acquire();
					if(valetManagerWaiting==1)
					{
						printf("\n The valet %d is reporting to Valet Manager",id);
						valetStatusLock[id]->Acquire();
						valetStatus[id]=-9;
						valetManagerWaiting--;
						CV_valetManagerWaiting->Signal(valetManagerWaitingLock);
						valetManagerWaitingLock->Release();
						CV_waitingForValetManager[id]->Wait(valetStatusLock[id]);
						if(valetStatus[id]==-10)
						{
							printf("\n Valet %d is in the back room ", id);
							CV_waitingInBackRoom[id]->Wait(valetStatusLock[id]);
							printf("\n Valet %d is out of the back room ", id);
						}
						valetStatusLock[id]->Release();
		
					}
					else
					{
							valetManagerWaitingLock->Release();

					}

		
			//--------------------------------------SERVICING LIMOS PARKING FIRST---------------------------------------------
	        limoQueueLengthLock->Acquire();
			 while(limoQueueLength>0)
            {
                limoQueueLength--;
                valetStatusLock[id]->Acquire();
                valetStatus[id]=-1; // SETS HIS STATUS TO BE FREE
                CV_limosWaitingToGetParked->Signal(limoQueueLengthLock); // SIGNALS A CAR
                limoQueueLengthLock->Release();
                CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR
                carToService=valetStatus[id]; // WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE
                printf("\n Valet %d is busy with Limousine %d",id,carToService);
                valetStatusLock[id]->Release();
                valetReadyWithCarLock[carToService]->Acquire();  // NOW THE VALET IS READY TO SERVICE A PARTICULAR CAR
                valetReadyWithCar[carToService]=1;
                CV_valetReadyWithCar[carToService]->Signal(valetReadyWithCarLock[carToService]); // SO HE SIGNALS THE DRIVER THAT HE IS WITH HIS CAR
                valetReadyWithCarLock[carToService]->Release();
                readyToParkLock[carToService]->Acquire();
                while(readyToPark[carToService]!=1) // THEN CHECKS IF THE CAR IS READY TO PARK
                {
                    //CAR IS NOT READY TO PARK SO HE WAITS FOR IT TO BECOME READY
                    printf("\n Valet %d is waiting to park Limousine %d",id,carToService);
                    CV_waitingToParkCar[carToService]->Wait(readyToParkLock[carToService]);

                }
                // THE CAR IS NOW READY TO PARK
                readyToParkLock[carToService]->Release();
                printf("\n Valet %d got the keys for the Limousine %d",id,carToService); // GETS THE KEYS
				parkingTokenLock[carToService]->Acquire();
				parkingToken[carToService]=carToService;
				CV_parkingToken[carToService]->Signal(parkingTokenLock[carToService]);
				printf("\n Valet %d is giving the parking token to Limosuine driver %d",id,carToService);
				parkingTokenLock[carToService]->Release();
                printf("\n Valet %d is parking Limousine %d",id,carToService); // PARKS THE CAR
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
				carParkedLock[carToService]->Acquire();
				carParked[carToService]=carToService;
				carParkedLock[carToService]->Release();
				limoQueueLengthLock->Acquire();
            }
			limoQueueLengthLock->Release();
	
			//--------------------------------------SERVICING CARS PARKING FIRST---------------------------------------------
			carQueueLengthLock->Acquire();
			while(carQueueLength>0)
			{
				carQueueLength--; // DECREMENTS THE CAR QUEUE LENGTH SINCE HE IS GOIN TO SIGNAL A CAR              
                valetStatusLock[id]->Acquire();
                valetStatus[id]=-2; // SETS HIS STATUS TO BE FREE
                CV_waitingToGetParked->Signal(carQueueLengthLock); // SIGNALS A CAR
                carQueueLengthLock->Release();
                CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR
                carToService=valetStatus[id]; // WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE
                printf("\n Valet %d is busy with car %d",id,carToService);
                valetStatusLock[id]->Release();
                valetReadyWithCarLock[carToService]->Acquire();  // NOW THE VALET IS READY TO SERVICE A PARTICULAR CAR
                valetReadyWithCar[carToService]=1;
                CV_valetReadyWithCar[carToService]->Signal(valetReadyWithCarLock[carToService]); // SO HE SIGNALS THE DRIVER THAT HE IS WITH HIS CAR
                valetReadyWithCarLock[carToService]->Release();
                readyToParkLock[carToService]->Acquire();
				carParkedLock[carToService]->Acquire();
                while(readyToPark[carToService]!=1) // THEN CHECKS IF THE CAR IS READY TO PARK
                {
                        //CAR IS NOT READY TO PARK SO HE WAITS FOR IT TO BECOME READY
                        printf("\n Valet %d is waiting to park car %d",id,carToService);
                        CV_waitingToParkCar[carToService]->Wait(readyToParkLock[carToService]);
                }
                // THE CAR IS NOW READY TO PARK
                readyToParkLock[carToService]->Release();
                printf("\n Valet %d got the keys for the car %d",id,carToService); // GETS THE KEYS
				parkingTokenLock[carToService]->Acquire();
				parkingToken[carToService]=carToService;
				CV_parkingToken[carToService]->Signal(parkingTokenLock[carToService]);
				printf("\n Valet %d is giving the parking token to driver %d",id,carToService);
				parkingTokenLock[carToService]->Release();
                printf("\n Valet %d is parking car %d",id,carToService); // PARKS THE CAR
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
				carParkedLock[carToService]->Acquire();
				carParked[carToService]=carToService;
				carParkedLock[carToService]->Release();
				carQueueLengthLock->Acquire();
			}
			carQueueLengthLock->Release();

			//--------------------------------------SERVICING EXIT LIMOS THIRD -------------------------------------------
			exitLimoQueueLengthLock->Acquire();
			while(exitLimoQueueLength>0)
			{
				exitLimoQueueLength--;
                valetStatusLock[id]->Acquire();
                valetStatus[id]=-3; // SETS HIS STATUS TO BE FREE
				CV_exitLimoQueueLength->Signal(exitLimoQueueLengthLock);
				exitLimoQueueLengthLock->Release();
                CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR
                carToService=valetStatus[id]; // WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE
                printf("\n Valet %d is busy with Limosuine %d",id,carToService);
                valetStatusLock[id]->Release();
				carParkedLock[carToService]->Acquire();
				if(carParked[carToService]==carToService)
				{
					printf("\n Valet %d got the token from Limousine driver %d,",id,carToService);
					carParkedLock[carToService]->Release();
					printf("\n Valet %d is bringing back the Limousine %d",id,carToService);
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					carReadyToGoLock[carToService]->Acquire();
					carReadyToGo[carToService]=1;
					CV_carReadyToGo[carToService]->Signal(carReadyToGoLock[carToService]);
					carReadyToGoLock[carToService]->Release();
				}
				tipsForThisCarLock[carToService]->Acquire();
				while(tipsForThisCar[carToService]<=0)
				{
					printf("\n Valet %d is waiting to get tips from the Limousine driver %d",id,carToService);
					CV_tipsForThisCar[carToService]->Wait(tipsForThisCarLock[carToService]);
				}
				printf("\n Valet %d has got the tip from Limousine driver %d of amount $ %d \n ",id,carToService,tipsForThisCar[carToService]);
				tipsForThisCarLock[carToService]->Release();
				carKeysReadyLock[carToService]->Acquire();
				carKeysReady[carToService]=1;
				printf("\n Valet %d is giving the key to Limousine %d",id,carToService);
				CV_carKeysReady[carToService]->Signal(carKeysReadyLock[carToService]);
				carKeysReadyLock[carToService]->Release();
				countOfExitedCarLock->Acquire();
				countOfExitedCars++;
				countOfExitedCarLock->Release();
				exitCarQueueLengthLock->Acquire();
			}
			exitLimoQueueLengthLock->Release();
			//--------------------------------------SERVICING EXIT CARS FOURTH--------------------------------------------
			exitCarQueueLengthLock->Acquire();
			while(exitCarQueueLength>0)
			{
				exitCarQueueLength--;
                valetStatusLock[id]->Acquire();
                valetStatus[id]=-4; // SETS HIS STATUS TO BE FREE
				CV_exitCarQueueLength->Signal(exitCarQueueLengthLock);
				exitCarQueueLengthLock->Release();
                CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR
                carToService=valetStatus[id]; // WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE
                printf("\n Valet %d is busy with car %d",id,carToService);
                valetStatusLock[id]->Release();
				carParkedLock[carToService]->Acquire();
				if(carParked[carToService]==carToService)
				{
					carParkedLock[carToService]->Release();
					printf("\n Valet %d is bringing back the car %d",id,carToService);
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					carReadyToGoLock[carToService]->Acquire();
					carReadyToGo[carToService]=1;
					CV_carReadyToGo[carToService]->Signal(carReadyToGoLock[carToService]);
					carReadyToGoLock[carToService]->Release();
				}
				tipsForThisCarLock[carToService]->Acquire();
				while(tipsForThisCar[carToService]<=0)
				{
					printf("\nValet %d is waiting to get tips from the driver %d",id,carToService);
					CV_tipsForThisCar[carToService]->Wait(tipsForThisCarLock[carToService]);
				}
				printf("\n Valet %d has got the tip from driver %d of amount $ %d \n ",id,carToService,tipsForThisCar[carToService]);
				tipsForThisCarLock[carToService]->Release();
				carKeysReadyLock[carToService]->Acquire();
				carKeysReady[carToService]=1;
				printf("\n Valet %d is giving the key to driver %d",id,carToService);
				CV_carKeysReady[carToService]->Signal(carKeysReadyLock[carToService]);
				carKeysReadyLock[carToService]->Release();
				countOfExitedCarLock->Acquire();
				countOfExitedCars++;
				countOfExitedCarLock->Release();
				exitCarQueueLengthLock->Acquire();
			}
			exitCarQueueLengthLock->Release();
	}
}


//-----------------------------------------------------------------------------------------------------------------------------------------------
// THIS IS THE LIMO SINE DRIVER FUNCTION DRIVER FUNCTION WHICH DOES THE WORK OF THE LIMO DRIVER
// THIS FUNCITON TAKES AN INTEGER ARGUEMENT WHICH IS THE IDENTIFIER OF THE DRIVER
//-----------------------------------------------------------------------------------------------------------------------------------------------

void limoDriver(int i)
{
    int id=i; // DRIVER RECORDS HIS IDENTITY
	int myParkingToken=-1;
    limoQueueLengthLock->Acquire();
    limoQueueLength++; // HE INCREMENTS THE CAR QUEUE LENGTH
    printf("\n Limousine %d is waitig in line and the number of waiting Limousines is %d",id,limoQueueLength);
    waitingValetCountLock->Acquire();
    if(countOfWaitingValets>0) // CHECKS IF ANY VALET IS IN BENCH
    {
        // THERE IS ONE FOUND IN THE BENCH SO THE DRIVER SIGNALS HIM
		CV_countOfWaitingValets->Signal(waitingValetCountLock);
    }
    waitingValetCountLock->Release();
	CV_limosWaitingToGetParked->Wait(limoQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED
    limoQueueLengthLock->Release();
    // ITS THE DRIVER CHANCE NOW
    for(int j=0;j<totalNumberOfValets;j++)
    {
        // SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE
        valetStatusLock[j]->Acquire();
        if(valetStatus[j]==-1)
        {
            // ONE IS FOUND TO BE FREE
			 valetStatus[j]=id;
            // CAR DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM
            CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);
            valetStatusLock[j]->Release();
            break;
        }
        valetStatusLock[j]->Release();
    }
    valetReadyWithCarLock[id]->Acquire();
    while(valetReadyWithCar[id]!=1)// CHECKS IF THE VALET IS READY TO PARK HIS CAR
    {
            // THE VALET IS NOT READY TO PARK THE CARS SO HE WAITS FOR THE VALET TO ARRIVE
            CV_valetReadyWithCar[id]->Wait(valetReadyWithCarLock[id]);

    }
    // THE VALET HAS ARRIVED
    valetReadyWithCarLock[id]->Release();
    visitorPleaseGetOutLock[id]->Acquire();
    visitorPleaseGetOut[id]=1;
    // BROADCASTS THE VISITORS IN THE CAR TO GET OUT
    CV_visitorPleaseGetOut[id]->Broadcast(visitorPleaseGetOutLock[id]);
    visitorPleaseGetOutLock[id]->Release();
    visitorCountLock[id]->Acquire();
    if(visitorCount[id]!=0)// CHECK IF ALL THE VISITORS HAVE GOT OUT
    {
        // NO STILL SOME ARE IN THE CAR SO HE WAITS
        CV_visitorCount[id]->Wait(visitorCountLock[id]);
    }
    // OK NOW NO ONE IS THE CAR EXCEPT HIM
    visitorCountLock[id]->Release();
    readyToParkLock[id]->Acquire();
    readyToPark[id]=1; // INDICATES THE CAR IS READY TO PARK
    CV_waitingToParkCar[id]->Signal(readyToParkLock[id]); //SIGNALS THE DRIVER TO PARK THE CAR
    readyToParkLock[id]->Release();
    // DRIVER IS GETTING OUT
    printf("\n Limousine driver %d is gettting out of the Limousine %d and giving the keys to Valet ",id,id);
    // GOIN TO THE MUSEUM
	parkingTokenLock[id]->Acquire();
	while(parkingToken[id]!=id)
	{
		printf("\n Limousine driver %d is waiting to get his parking token from the Valet",id);
		CV_parkingToken[id]->Wait(parkingTokenLock[id]);
	}
	myParkingToken=parkingToken[id];
	printf("\n Limousine driver %d got his parking token and the number on it is %d",id,myParkingToken);
	parkingTokenLock[id]->Release();
	//----------------------------------- LIMO DRIVER WAITING FOR VISITORS AND INTERACTING WITH VALET TO GET HIS CAR BACK -----------------------//
	driverStatusLock[id]->Acquire();
	driverStatus[id]=1;
	visitorsReadyToGoHomeLock[id]->Acquire();
	printf("\n Limousine driver %d is checking if all the visitos have come or not ",id);
	while(visitorsReadyToGoHome[id]<totalNumberOfVisitors[id])
	{
		visitorsReadyToGoHomeLock[id]->Release();
		printf("\n Limousine driver %d is saying I am waiting for the visitors in my car to arrive \n",id);
		CV_driverStatus[id]->Wait(driverStatusLock[id]);
		visitorsReadyToGoHomeLock[id]->Acquire();
	}
	visitorsReadyToGoHomeLock[id]->Release();
	driverStatusLock[id]->Release();
	printf("\n Limousine driver %d says ok all the visitors in  my car has arrived im goin to start my interaction with the valet now \n",id);
	
	// SHUD RITE CODE HERE TO INFORM THE VALET
	//----------------------------- DRIVER IS COMING BACK END ----------------------------------------------------------------//
	exitLimoQueueLengthLock->Acquire();
	exitLimoQueueLength++;
    printf("\n Number of waiting Limousines to exit is is %d",exitLimoQueueLength);
    waitingValetCountLock->Acquire();
    if(countOfWaitingValets>0) // CHECKS IF ANY VALET IS IN BENCH
    {
        CV_countOfWaitingValets->Signal(waitingValetCountLock);
    }
    waitingValetCountLock->Release();
    CV_exitLimoQueueLength->Wait(exitLimoQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED
    exitLimoQueueLengthLock->Release();
    for(int j=0;j<totalNumberOfValets;j++)
    {
        // SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE
        valetStatusLock[j]->Acquire();
        if(valetStatus[j]==-3)
        {
            // ONE IS FOUND TO BE FREE
            valetStatus[j]=myParkingToken;
            // LIMO DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM
            CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);
            valetStatusLock[j]->Release();
            break;
        }
        valetStatusLock[j]->Release();
    }
	carReadyToGoLock[id]->Acquire();
	while(carReadyToGo[id]!=1)
	{
		printf("\n Limousine driver %d is waiting for the Valet to get his car \n ", id);
		CV_carReadyToGo[id]->Wait(carReadyToGoLock[id]);
	}
	printf("\n Limousine driver %d says the valet has come back with my car \n",id);
	carReadyToGoLock[id]->Release();
	tipsForThisCarLock[id]->Acquire();
	printf("\n Limousine driver %d says i am giving tip to the valet\n",id);
	tipsForThisCar[id]=1;
	CV_tipsForThisCar[id]->Signal(tipsForThisCarLock[id]);
	tipsForThisCarLock[id]->Release();
	carKeysReadyLock[id]->Acquire();
	while(carKeysReady[id]!=1)
	{
		printf("\n Limousine driver %d is waiting to get his keys \n",id);
		CV_carKeysReady[id]->Wait(carKeysReadyLock[id]);
	}
	printf("\n Limousine driver %d say i have the key to my car \n",id);
	carKeysReadyLock[id]->Release();
	visitorPleaseGetInLock[id]->Acquire();
	visitorPleaseGetIn[id]=1;
	visitorPleaseGetInLock[id]->Release();
	visitorsReadyToGoHomeLock[id]->Acquire();
	CV_visitorReadyToGoHome[id]->Broadcast(visitorsReadyToGoHomeLock[id]);
	visitorCountLock[id]->Acquire();
	visitorsReadyToGoHomeLock[id]->Release();
	while(visitorCount[id]<totalNumberOfVisitors[id])
	{
		CV_visitorPleaseGetIn[id]->Wait(visitorCountLock[id]);
	}
	visitorCountLock[id]->Release();
	printf("\n Limousine driver %d says to the all the vistors in his limousine that we are all going out of the museum",id);

	//-------------------------------------------------------------------------------------------------------------------------------------------//
   
}




//-----------------------------------------------------------------------------------------------------------------------------------------------
// THIS IS THE DRIVER FUNCTION WHICH DOES THE WORK OF THE DRIVER
// THIS FUNCITON TAKES AN INTEGER ARGUEMENT WHICH IS THE IDENTIFIER OF THE DRIVER
//-----------------------------------------------------------------------------------------------------------------------------------------------
void driver(int i)
{
    int id=i; // DRIVER RECORS HIS IDENTITY
	int myParkingToken=-1;
	int myTicketTaker=-1;
	int leastLength;
    carQueueLengthLock->Acquire();
    carQueueLength++; // HE INCREMENTS THE CAR QUEUE LENGTH
    printf("\n Number of waiting cars is %d",carQueueLength);
    waitingValetCountLock->Acquire();
    if(countOfWaitingValets>0) // CHECKS IF ANY VALET IS IN BENCH
    {
        // THERE IS ONE FOUND IN THE BENCH SO THE DRIVER SIGNALS HIM
        CV_countOfWaitingValets->Signal(waitingValetCountLock);
    }
    waitingValetCountLock->Release();
    CV_waitingToGetParked->Wait(carQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED
    carQueueLengthLock->Release();
    // ITS THE DRIVER CHANCE NOW
    for(int j=0;j<totalNumberOfValets;j++)
    {
        // SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE
        valetStatusLock[j]->Acquire();
        if(valetStatus[j]==-2)
        {
            // ONE IS FOUND TO BE FREE
            valetStatus[j]=id;
            // CAR DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM
            CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);
            valetStatusLock[j]->Release();
            break;
        }
		else
		{
			valetStatusLock[j]->Release();
		}
    }
    valetReadyWithCarLock[id]->Acquire();
    while(valetReadyWithCar[id]!=1)// CHECKS IF THE VALET IS READY TO PARK HIS CAR
    {
            // THE VALET IS NOT READY TO PARK THE CARS SO HE WAITS FOR THE VALET TO ARRIVE
            CV_valetReadyWithCar[id]->Wait(valetReadyWithCarLock[id]);

    }
    // THE VALET HAS ARRIVED
    valetReadyWithCarLock[id]->Release();
    visitorPleaseGetOutLock[id]->Acquire();
    visitorPleaseGetOut[id]=1;
    // BROADCASTS THE VISITORS IN THE CAR TO GET OUT
    CV_visitorPleaseGetOut[id]->Broadcast(visitorPleaseGetOutLock[id]);
    visitorPleaseGetOutLock[id]->Release();
    visitorCountLock[id]->Acquire();
    if(visitorCount[id]!=0)// CHECK IF ALL THE VISITORS HAVE GOT OUT
    {
        // NO STILL SOME ARE IN THE CAR SO HE WAITS
        CV_visitorCount[id]->Wait(visitorCountLock[id]);
    }
    // OK NOW NO ONE IS THE CAR EXCEPT HIM
    visitorCountLock[id]->Release();
    readyToParkLock[id]->Acquire();
    readyToPark[id]=1; // INDICATES THE CAR IS READY TO PARK
    CV_waitingToParkCar[id]->Signal(readyToParkLock[id]); //SIGNALS THE DRIVER TO PARK THE CAR
    readyToParkLock[id]->Release();
    // DRIVER IS GETTING OUT
    printf("\n Driver %d is gettting out of the car %d and giving the keys to Valet ",id,id);
	parkingTokenLock[id]->Acquire();
	while(parkingToken[id]!=id)
	{
		printf("\n Driver %d is waiting to get his parking token from the Valet",id);
		CV_parkingToken[id]->Wait(parkingTokenLock[id]);
	}
	myParkingToken=parkingToken[id];
	printf("\n Driver %d got his parking token and the number on it is %d",id,myParkingToken);
	parkingTokenLock[id]->Release();
    // GOIN TO THE MUSEUM
    printf("\n Driver%d is goin to the museum",id);

	//---------------------------- DRIVER GOIN TO MUSUEM CODE SEGMENT -----------------------------------------------------//
	// -----------------START OF DRVIER TICKET TAKER INTERACTION CODE-------------------------------
	 totalNumberOfActiveTicketTakersLock->Acquire();
	 if(totalNumberOfActiveTicketTakers==0)
	{
		printf("\n Driver %d is waiting since there are no active ticker takers ", id);
        CV_noTicketTakers->Wait(totalNumberOfActiveTicketTakersLock);
	}
	for(int j=0;j<totalNumberOfActiveTicketTakers;j++)
	{
		totalNumberOfActiveTicketTakersLock->Release();
		ticketTakerQueueLengthLock[j]->Acquire();
		if(ticketTakerQueueLength[j]==0)
		{
			myTicketTaker=j;
			ticketTakerQueueLengthLock[j]->Release();
			break;
		}
		if(j==0)
		{
			myTicketTaker=j;
			leastLength=ticketTakerQueueLength[j];
		}
		else
		{
			if(leastLength<ticketTakerQueueLength[j])
			{
				myTicketTaker=j;
				leastLength=ticketTakerQueueLength[j];
			}
		}
	ticketTakerQueueLengthLock[j]->Release();
	totalNumberOfActiveTicketTakersLock->Acquire();
	}
	totalNumberOfActiveTicketTakersLock->Release();
	ticketTakerQueueLengthLock[myTicketTaker]->Acquire();
	ticketTakerQueueLength[myTicketTaker]++;
	visitorMuseumEntryStatusLock[myTicketTaker]->Acquire();
	CV_waitingTicketTaker[myTicketTaker]->Signal(ticketTakerQueueLengthLock[myTicketTaker]);
	ticketTakerQueueLengthLock[myTicketTaker]->Release();
	if(visitorMuseumEntryStatus[myTicketTaker]!=1)
	{
			printf("\n Driver %d is waiting in the line of Ticket Taker %d",id,myTicketTaker);
		    CV_visitorMuseumEntryStatus[myTicketTaker]->Wait(visitorMuseumEntryStatusLock[myTicketTaker]);
	}
	visitorMuseumEntryStatus[myTicketTaker]=0;
	visitorMuseumEntryStatusLock[myTicketTaker]->Release();
	printf("\n Driver %d got his ticket accepdted ",id);
	printf("\n Driver %d is inside the museum",id);
	//-------------------END OF DRIVER TICKET TAKER INTERACTION CODE --------------------------------------
	//---------------------------- DRIVER GOIN TO MUSUEM CODE SEGMENT -- END -----------------------------------------------------//

	//----------------------------- DRIVER IS COMING BACK ---------------------------------------------------------------------//
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	printf("\n Driver %d says ok im done seeing the museum im goin back to get my car \n ",id);
	driverStatusLock[id]->Acquire();
	driverStatus[id]=1;
	visitorsReadyToGoHomeLock[id]->Acquire();
	printf("\n Driver %d is checking if all the visitos have come or not \n",id);
	while(visitorsReadyToGoHome[id]<totalNumberOfVisitors[id])
	{
		visitorsReadyToGoHomeLock[id]->Release();
		printf("\n Driver %d is saying I am waiting for the visitos in my car to arrive \n",id);
		CV_driverStatus[id]->Wait(driverStatusLock[id]);
		visitorsReadyToGoHomeLock[id]->Acquire();
	}
	visitorsReadyToGoHomeLock[id]->Release();
	driverStatusLock[id]->Release();
	printf("\n Driver %d says ok all the visitors in  my car has arrived im goin to start my interaction with the valet now \n",id);
	
	// SHUD RITE CODE HERE TO INFORM THE VALET
	//----------------------------- DRIVER IS COMING BACK END ----------------------------------------------------------------//
	exitCarQueueLengthLock->Acquire();
	exitCarQueueLength++;
    printf("\n Number of waiting cars to exit is %d",exitCarQueueLength);
    waitingValetCountLock->Acquire();
    if(countOfWaitingValets>0) // CHECKS IF ANY VALET IS IN BENCH
    {
        CV_countOfWaitingValets->Signal(waitingValetCountLock);
    }
    waitingValetCountLock->Release();
    CV_exitCarQueueLength->Wait(exitCarQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED
    exitCarQueueLengthLock->Release();
    for(int j=0;j<totalNumberOfValets;j++)
    {
        // SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE
        valetStatusLock[j]->Acquire();
        if(valetStatus[j]==-4)
        {
            // ONE IS FOUND TO BE FREE
            valetStatus[j]=myParkingToken;
            // CAR DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM
            CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);
            valetStatusLock[j]->Release();
            break;
        }
		else
		{
			valetStatusLock[j]->Release();
		}
    }
	carReadyToGoLock[id]->Acquire();
	while(carReadyToGo[id]!=1)
	{
		printf("\n Driver %d says im waiting for the valet to get my car \n ", id);
		CV_carReadyToGo[id]->Wait(carReadyToGoLock[id]);
	}
	printf("\n Driver %d says the valet has come back with my car \n",id);
	carReadyToGoLock[id]->Release();
	tipsForThisCarLock[id]->Acquire();
	printf("\n Driver %d says i am giving tip to the valet \n",id);
	tipsForThisCar[id]=1;
	CV_tipsForThisCar[id]->Signal(tipsForThisCarLock[id]);
	tipsForThisCarLock[id]->Release();
	carKeysReadyLock[id]->Acquire();
	while(carKeysReady[id]!=1)
	{
		printf("\n Driver %d is waiting to get his keys \n",id);
		CV_carKeysReady[id]->Wait(carKeysReadyLock[id]);
	}
	printf("\n Driver %d say i have the key to my car \n",id);
	carKeysReadyLock[id]->Release();
	visitorPleaseGetInLock[id]->Acquire();
	visitorPleaseGetIn[id]=1;
	visitorPleaseGetInLock[id]->Release();
	visitorsReadyToGoHomeLock[id]->Acquire();
	CV_visitorReadyToGoHome[id]->Broadcast(visitorsReadyToGoHomeLock[id]);
	visitorCountLock[id]->Acquire();
	visitorsReadyToGoHomeLock[id]->Release();
	while(visitorCount[id]<totalNumberOfVisitors[id])
	{
		CV_visitorPleaseGetIn[id]->Wait(visitorCountLock[id]);
	}
	visitorCountLock[id]->Release();
	printf("\n Driver %d says to all the vistors in his car that we are all going out of the museum",id);
}


//-----------------------------------------------------------------------------------------------------------------------------------------------
// THIS IS THE VISITOR FUNCTION WHICH DOES THE WORK OF THE VISITOR
// THIS FUNCITON TAKES AN INTEGER ARGUEMENT WHICH IS THE IDENTIFIER OF THE VISITOR
//-----------------------------------------------------------------------------------------------------------------------------------------------
void visitorFunction(int i)
{

    int id=i;// RECORS THEIR IDENTITY
    int myCar=visitor[id]; // RECORDS THE CAR TO WHICH THEY BELONG
    int myTicketTaker=-1; // INTIALLY THE VISITOR IS NOT WITH ANY TICKET TAKER
	int leastLength;
	char *vehicle=new char[10];
	if(myCar<totalNumberOfCars)
	{
		sprintf(vehicle,"Car%d",myCar);
	}
	else
	{
		sprintf(vehicle,"Limousine%d",myCar);
	}
    visitorPleaseGetOutLock[myCar]->Acquire();
    if(visitorPleaseGetOut[myCar]!=1) // CHECKS IF THERE IS ANY INDICATION FROM THE DRIVER ASKING THEM TO GET OUT OF THE CAR
    {
        // NO THERE IS NO INDICATION SO THE VISITOR IS WAITING
        printf("\n Visitor %d is waiting to get out of the %s \n",id,vehicle);
        CV_visitorPleaseGetOut[myCar]->Wait(visitorPleaseGetOutLock[myCar]);
    }
    visitorPleaseGetOutLock[myCar]->Release();
    // OK NOW THEY HAVE RECIVED AN INDICATION FROM THE DRIVER
    visitorCountLock[myCar]->Acquire();
    visitorCount[myCar]--; //SINCE THEY ARE GETTING OUT THEY ARE DECREMENTING THE COUNT OF VISITORS IN CAR
    //printf("\n -------------------------------------------------------------------> visitor count now is :%d \n ",visitorCount[myCar]);
    if(visitorCount[myCar]==0)// CHECKS IF THE VISITOR IS THE LAST VISITOR
    {
        // YES THE VISITOR IS THE LASR VISITOR SO HE SIGNALS THE DRIVER
        CV_visitorCount[myCar]->Signal(visitorCountLock[myCar]);
        printf("\n Visitor %d signaling his driver that everybody is out of the car \n",id);
    }
  
    printf("\n Visitor %d is goin to the museum \n",id);
     visitorCountLock[myCar]->Release();
	 // VISITOR GOES TO THE MUESUM

	 // -----------------START OF   VISITOR TICKET TAKER INTERACTION CODE-------------------------------
	 totalNumberOfActiveTicketTakersLock->Acquire();
	 if(totalNumberOfActiveTicketTakers==0)
	{
		printf("\n Visitor %d is waiting since there are no active Ticker Takers ", id);
        CV_noTicketTakers->Wait(totalNumberOfActiveTicketTakersLock);
	}
	for(int j=0;j<totalNumberOfActiveTicketTakers;j++)
	{
		totalNumberOfActiveTicketTakersLock->Release();
		ticketTakerQueueLengthLock[j]->Acquire();
		if(ticketTakerQueueLength[j]==0)
		{
			myTicketTaker=j;
			ticketTakerQueueLengthLock[j]->Release();
			break;
		}
		if(j==0)
		{
			myTicketTaker=j;
			leastLength=ticketTakerQueueLength[j];
		}
		else
		{
			if(leastLength<ticketTakerQueueLength[j])
			{
				myTicketTaker=j;
				leastLength=ticketTakerQueueLength[j];
			}
		}
	ticketTakerQueueLengthLock[j]->Release();
	totalNumberOfActiveTicketTakersLock->Acquire();
	}
	totalNumberOfActiveTicketTakersLock->Release();
	ticketTakerQueueLengthLock[myTicketTaker]->Acquire();
	ticketTakerQueueLength[myTicketTaker]++;
	visitorMuseumEntryStatusLock[myTicketTaker]->Acquire();
	CV_waitingTicketTaker[myTicketTaker]->Signal(ticketTakerQueueLengthLock[myTicketTaker]);
	ticketTakerQueueLengthLock[myTicketTaker]->Release();
	if(visitorMuseumEntryStatus[myTicketTaker]!=1)
	{
			printf("\n Visitor %d is waiting in the line of  Ticket Taker %d",id,myTicketTaker);
		    CV_visitorMuseumEntryStatus[myTicketTaker]->Wait(visitorMuseumEntryStatusLock[myTicketTaker]);
	}
	visitorMuseumEntryStatus[myTicketTaker]=0;
	visitorMuseumEntryStatusLock[myTicketTaker]->Release();
	printf("\n Visitor %d got his ticket accepdted ",id);
	printf("\n Visitor %d is inside the museum",id);
	//-------------------END OF VISITOR TICKET TAKER INTERACTION CODE --------------------------------------
	currentThread->Yield();
	currentThread->Yield();
	printf("\n Visitor %d say ok fine im done seeing the museum im goin back to my car \n",id);
	// ALRIGHT THE VISITORS ARE WAITING TO GO HOME 
	visitorsReadyToGoHomeLock[myCar]->Acquire();
	visitorsReadyToGoHome[myCar]++;
	//printf("\n %d out of %d ppl are back now \n",visitorsReadyToGoHome[myCar],totalNumberOfVisitors[myCar]);
	if(visitorsReadyToGoHome[myCar]<totalNumberOfVisitors[myCar])
	{
				CV_visitorReadyToGoHome[myCar]->Wait(visitorsReadyToGoHomeLock[myCar]);
	}
	else
	{
		//printf("\n the total number of visitors waiting to get back on the car %d is %d \n",myCar,visitorsReadyToGoHome[myCar]);
		driverStatusLock[myCar]->Acquire();
		printf("\n Visitor %d is checking if Driver has arrived \n",id);
		if(driverStatus[myCar]==1)
		{
			printf("\n Visitor %d says yes the driver is already there \n",id);
			CV_driverStatus[myCar]->Signal(driverStatusLock[myCar]);
			driverStatusLock[myCar]->Release();
			CV_visitorReadyToGoHome[myCar]->Wait(visitorsReadyToGoHomeLock[myCar]);
		}
		else
		{
			driverStatusLock[myCar]->Release();
			printf("\n Visitor %d says nope still no driver I have to wait \n",id);
			CV_visitorReadyToGoHome[myCar]->Wait(visitorsReadyToGoHomeLock[myCar]);
		}

	}
	visitorsReadyToGoHomeLock[myCar]->Release();
	visitorCountLock[myCar]->Acquire();
	visitorCount[myCar]++;
	if(visitorCount[myCar]==totalNumberOfVisitors[myCar])
	{
		//printf("\n the count of visitors in the car %d and the total count is %d",visitorCount[myCar],totalNumberOfVisitors[myCar]);
		printf("\n Visitor %d is signaling driver we are all in ",id);
		CV_visitorPleaseGetIn[myCar]->Signal(visitorCountLock[myCar]);
	}
	visitorCountLock[myCar]->Release();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------
// THIS IS THE TICKET TAKER FUNCTION WHICH DOES THE WORK OF THE TICKET TAKER
// THIS FUNCITON TAKES AN INTEGER ARGUEMENT WHICH IS THE IDENTIFIER OF THE TICKER TAKER
//-----------------------------------------------------------------------------------------------------------------------------------------------
void ticketTakerFunction(int i)
{
    int id=i; // RECORDS HIS IDENTITY
    totalNumberOfActiveTicketTakersLock->Acquire();
	totalNumberOfActiveTicketTakers++;
    CV_noTicketTakers->Broadcast(totalNumberOfActiveTicketTakersLock);
	totalNumberOfActiveTicketTakersLock->Release();
	while(1)
	{
		ticketTakerQueueLengthLock[id]->Acquire();
		if(ticketTakerQueueLength[id]==0)
		{
			printf("\n Ticket Taker %d finds no visitor in his queue so he waits ",id);
			visitorMuseumEntryStatusLock[id]->Acquire();
			visitorMuseumEntryStatus[id]=0;
			visitorMuseumEntryStatusLock[id]->Release();
			CV_waitingTicketTaker[id]->Wait(ticketTakerQueueLengthLock[id]);
		}
		while(ticketTakerQueueLength[id]>0)
		{
			visitorMuseumEntryStatusLock[id]->Acquire();
			visitorMuseumEntryStatus[id]=1;
			CV_visitorMuseumEntryStatus[id]->Signal(visitorMuseumEntryStatusLock[id]);
			ticketTakerQueueLength[id]--;
			printf("\n Ticket taker %d accepting the ticket from a visitor",id); 
			visitorMuseumEntryStatusLock[id]->Release();
		}
		ticketTakerQueueLengthLock[id]->Release();

	}
    
}

//----------------------------------------------------------------------------------
// THIS IS THE FUNCITON WHICH PRINTS THE DEATILS ABOUT THE  CAR - USED FOR DEBUGING
//-----------------------------------------------------------------------------------
void printCarDetails(int i)
{
	int id=i;
	char *vehicle=new char[10];
	if(id<totalNumberOfCars)
	{
		sprintf(vehicle,"Car%d",id);
	}
	else
	{
		sprintf(vehicle,"Limousine%d",id);
	}
    printf("\n");
    printf("\n the %s number is :%d",vehicle,id);
    printf("\n the driver number of this %s is %d:",vehicle,id);
    printf("\n the ready to park value of this %s is %d",vehicle,readyToPark[id]);
    printf("\n the visitor please get out value of this %s is :%d",vehicle,visitorPleaseGetOut[id]);
    printf("\n the total number of visitors in this %s is :%d",vehicle,totalNumberOfVisitors[id]);
    printf("\n the current visitor count in this %s is :%d",vehicle,visitorCount[id]);
    printf("\n the car parked value of this %s is :%d",vehicle,carParked[id]);
    printf("\n the parking token of this %s is :%d",vehicle,parkingToken[id]);
    printf("\n");
}

//----------------------------------------------------------------------------------
// THIS IS THE FUNCITON WHICH PRINTS THE DEATILS ABOUT THE  VALET - USED FOR DEBUGING
//-----------------------------------------------------------------------------------
void printValetDetails(int i)
{
    printf("\n");
    printf("\nthe valet number is :%d",i);
    printf("\nhis status is :%d",valetStatus[i]);
    printf("\n");
}


//----------------------------------------------------------------------------------
// THIS IS THE FUNCITON WHICH SETS UP ENVIRONMENT FOR SIMULATION.
// ALL THE LOCKS AND CONDITION VARIBALES ARE CREATED HERE.
//-----------------------------------------------------------------------------------
void setup()
{
    char *name;

    printf("\n Enter the total number of valets in simulation :");
    scanf("%d",&totalNumberOfValets);
	//totalNumberOfValets=2;

    printf("\n Enter the total number of cars in this simulation:");
    scanf("%d",&totalNumberOfCars);
	//totalNumberOfCars=2;

    totalNumberOfDrivers=totalNumberOfCars;

    printf("\n Enter the total number of limousines:");
    scanf("%d",&totalCountOfLimos);
	//totalCountOfLimos=2;
   
    totalCountOfLimoDrivers=totalCountOfLimos;

    printf("\n Please enter the total number of ticket takers");
    scanf("%d",&totalNumberOfTicketTakers);
	//totalNumberOfTicketTakers=2;


    //-------------------------CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT-----------------------------

	
	exitLimoQueueLengthLock=new Lock("exitLimoQueueLengthLock");
	CV_exitLimoQueueLength=new Condition("CV_exitLimoQueueLength");

	exitCarQueueLengthLock=new Lock("exitCarQueueLengthLock");
	CV_exitCarQueueLength=new Condition("CV_exitCarQueueLength");


    carQueueLengthLock=new Lock("carQueueLengthLock");
    CV_waitingToGetParked=new Condition("CV_waitingToGetParked");

    limoQueueLengthLock=new Lock("limoQueueLengthLock");
    CV_limosWaitingToGetParked=new Condition("CV_limosWaitingToGetParked");


    countOfExitedCarLock=new Lock("countOfExitedCarLock");

    waitingValetCountLock=new Lock("waitingValetCountLock");
    CV_countOfWaitingValets=new Condition("CV_countOfWaitingValets");

    readyToPark=new int[totalNumberOfCars+totalCountOfLimos];
    readyToParkLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_waitingToParkCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

    visitorPleaseGetOut=new int[totalNumberOfCars+totalCountOfLimos];
    visitorPleaseGetOutLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorPleaseGetOut=new Condition*[totalNumberOfCars+totalCountOfLimos];

    totalNumberOfVisitors=new int[totalNumberOfCars+totalCountOfLimos];
    totalNumberOfVisitorsLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    visitorCount=new int [totalNumberOfCars+totalCountOfLimos];
    visitorCountLock= new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorCount=new Condition*[totalNumberOfCars+totalCountOfLimos];

    carParked=new int[totalNumberOfCars+totalCountOfLimos];
    carParkedLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    parkingToken=new int[totalNumberOfCars+totalCountOfLimos];
    parkingTokenLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_parkingToken=new Condition*[totalNumberOfCars+totalCountOfLimos];

    valetReadyWithCar=new int[totalNumberOfCars+totalCountOfLimos];
    valetReadyWithCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_valetReadyWithCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	driverStatus=new int[totalNumberOfCars+totalCountOfLimos];	
	driverStatusLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_driverStatus=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorsReadyToGoHome=new int[totalNumberOfCars+totalCountOfLimos];
	visitorsReadyToGoHomeLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorReadyToGoHome=new Condition*[totalNumberOfCars+totalCountOfLimos];

	ticketTakerLock=new Lock("ticketTakerLock");

	tipsForThisCar=new int[totalNumberOfCars+totalCountOfLimos];
	tipsForThisCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_tipsForThisCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	carKeysReady=new int[totalNumberOfCars+totalCountOfLimos];	
	carKeysReadyLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carKeysReady=new Condition*[totalNumberOfCars+totalCountOfLimos];	

	carReadyToGo=new int[totalNumberOfCars+totalCountOfLimos];
	carReadyToGoLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carReadyToGo=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorPleaseGetIn=new int[totalNumberOfCars+totalCountOfLimos];
	visitorPleaseGetInLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorPleaseGetIn=new Condition*[totalNumberOfCars+totalCountOfLimos];




    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        readyToPark[i]=0;
        name=new char[50];
        sprintf(name,"readyToParkLock%d",i);
        readyToParkLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_waitingToParkCar%d",i);
        CV_waitingToParkCar[i]=new Condition(name);

        visitorPleaseGetOut[i]=0;
        name=new char[50];
        sprintf(name,"visitorPleaseGetOutLock%d",i);
        visitorPleaseGetOutLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorPleaseGetOut%d",i);
        CV_visitorPleaseGetOut[i]=new Condition(name);

        totalNumberOfVisitors[i]=rand()%5;
		if(i>=totalNumberOfCars)
		{
			totalNumberOfVisitors[i]++;
		}
        grandTotalOfVisitors=grandTotalOfVisitors+totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"totalNumberOfVisitors%d",i);
        totalNumberOfVisitorsLock[i]=new Lock(name);
   
        visitorCount[i]=totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"visitorCountLock%d",i);
        visitorCountLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorCount%d",i);
        CV_visitorCount[i]=new Condition(name);
       
        carParked[i]=-1;
        name=new char[50];
        sprintf(name,"carParkedLock%d",i);
        carParkedLock[i]=new Lock(name);

        parkingToken[i]=-1;
        name=new char[50];
        sprintf(name,"parkingTokenLock%d",i);
        parkingTokenLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_parkingToken%d",i);
		CV_parkingToken[i]=new Condition(name);

        valetReadyWithCar[i]=0;
        name=new char[50];
        sprintf(name,"valetReadyWithCarLock%d",i);
        valetReadyWithCarLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_valetReadyWithCar%d",i);
        CV_valetReadyWithCar[i]=new Condition(name);

		driverStatus[i]=0;
		name=new char[50];
		sprintf(name,"driverStatusLock%d",i);
		driverStatusLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_driverStatus%d",i);
		CV_driverStatus[i]=new Condition(name);

		visitorsReadyToGoHome[i]=0;
		name=new char[50];
		sprintf(name,"visitorsReadyToGoHomeLock%d",i);
		visitorsReadyToGoHomeLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorReadyToGoHome%d",i);
		CV_visitorReadyToGoHome[i]=new Condition(name);

		tipsForThisCar[i]=0;
		name=new char[50];
		sprintf(name,"tipsForThisCarLock%d",i);
		tipsForThisCarLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_tipsForThisCar%d",i);
		CV_tipsForThisCar[i]=new Condition(name);	
		
		carKeysReady[i]=0;
		name=new char[50];
		sprintf(name,"carKeysReadyLock%d",i);
		carKeysReadyLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carKeysReady%d",i);
		CV_carKeysReady[i]=new Condition(name);	


		carReadyToGo[i]=0;
		name=new char[50];
		sprintf(name,"carReadyToGo%d",i);
		carReadyToGoLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carReadyToGo%d",i);
		CV_carReadyToGo[i]=new Condition(name);

	
		visitorPleaseGetIn[i]=0;
		name=new char[50];
		sprintf(name,"visitorPleaseGetInLock%d",i);
		visitorPleaseGetInLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorPleaseGetIn%d",i);
		CV_visitorPleaseGetIn[i]=new Condition(name);



    }
    //--------------------END OF CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT--------------------


	CV_waitingForValetManager=new Condition*[totalNumberOfValets];

	CV_waitingInBackRoom=new Condition*[totalNumberOfValets];

	valetManagerWaitingLock=new Lock("valetManagerWaitingLock");
	CV_valetManagerWaiting=new Condition("CV_valetManagerWaiting");

    valetStatus=new int[totalNumberOfValets];
    valetStatusLock=new Lock*[totalNumberOfValets];
    CV_valetWaitingForDriver=new Condition*[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[50];
        valetStatus[i]=-5;
        sprintf(name,"valetStatusLock%d",i);
        valetStatusLock[i]=new Lock(name);
        //delete(name);
        name=new char[50];
        sprintf(name,"CV_valetWaitingForDriver%d",i);
        CV_valetWaitingForDriver[i]=new Condition(name);
        //delete(name);
		
		name=new char[50];
		sprintf(name,"CV_waitingForValetManager%d",i);
		CV_waitingForValetManager[i]=new Condition(name);

		name=new char[50];
		sprintf(name,"CV_waitingInBackRoom%d",i);
		CV_waitingInBackRoom[i]=new Condition(name);
    }
    //------------------ END OF CODE SEGMENT TO SET UP VALET------------------------------

    //----------------------------- CODE SEGMENT TO SET UP THE TICKET TAKERS ----------------
	
    //printf("\n The Total number of ticket takers in this simulation is %d",totalNumberOfTicketTakers);
   
    CV_noTicketTakers=new Condition("CV_noTicketTakers");
    totalNumberOfActiveTicketTakersLock=new Lock("totalNumberOfActiveTicketTakers");
    ticketTakerQueueLength=new int[totalNumberOfTicketTakers];
   
    ticketTakerQueueLengthLock=new Lock*[totalNumberOfTicketTakers];
   
    CV_waitingTicketTaker=new Condition*[totalNumberOfTicketTakers];

    CV_ticketTakerQueueLength=new Condition*[totalNumberOfTicketTakers];
    visitorMuseumEntryStatus=new int[totalNumberOfTicketTakers];
    visitorMuseumEntryStatusLock=new Lock*[totalNumberOfTicketTakers];
    CV_visitorMuseumEntryStatus=new Condition*[totalNumberOfTicketTakers];

    totalNumberOfVisitorsAcceptedLock=new Lock("totalNumberOfVisitorsAcceptedLock");
    if(totalNumberOfDrivers==0 && totalCountOfLimoDrivers==0)
    {
        totalNumberOfVisitorsAccepted=-1;
    }

    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
       
        ticketTakerQueueLength[i]=0; //SETTING THE INTIAL VALUE TO BE ZERO
        visitorMuseumEntryStatus[i]=0;
       
        name=new char[50];
        sprintf(name,"visitorMuseumEntryStatusLock%d",i);
        visitorMuseumEntryStatusLock[i]=new Lock(name);

        name=new char[50];
        sprintf(name,"CV_visitorMuseumEntryStatus%d",i);
        CV_visitorMuseumEntryStatus[i]=new Condition(name);

        name=new char[50];
        sprintf(name,"ticketTakerQueueLengthLock%d",i);
        ticketTakerQueueLengthLock[i]=new Lock(name);
       
        name=new char[50];
        sprintf(name,"CV_waitingTicketTaker%d",i);
        CV_waitingTicketTaker[i]=new Condition(name);
       
        name=new char[50];
        sprintf(name,"CV_ticketTakerQueueLength%d",i);
        CV_ticketTakerQueueLength[i]=new Condition(name);
       
    }
    //-------------------END OF CODE SEGMENT TO SET UP THE TICKET TAKERS -------------------------------
   

}


Thread *valetThread;
Thread *driverThread;
Thread *visitorThreadPtr;
Thread *ticketTakerThreadPtr;
Thread *limoDriverThread;
Thread *valetManagerThreadPtr;


//----------------------------------------------------------------------------------
// THIS IS PROBLEM 2 FUNCTION WHICH INTIATES A SIMULATION
// ALL THE THREADS ARE CREATED HERE
//-----------------------------------------------------------------------------------
void Problem2()
{
    char *name;
    printf("\n********************Simulation starting***************************");
    setup();

    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        printCarDetails(i);
    }
    for(int i=0;i<totalNumberOfValets;i++)
    {
        printValetDetails(i);
    }
    printf("\n");
    visitor=new int[grandTotalOfVisitors];
    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        static int k=0;
        if(totalNumberOfVisitors[i]!=0)
        {
                for(int j=0;j<totalNumberOfVisitors[i];j++)
                {
                    visitor[k]=i;   
                    k++;
                }
        }
    }
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
		char *vehicle=new char[10];
		if(visitor[i]<totalNumberOfCars)
		{
			sprintf(vehicle,"car%d",visitor[i]);
			printf("\nvisitor %d is in  %s",i,vehicle);
		}
		else
		{
			sprintf(vehicle,"Limousine%d",visitor[i]);
			printf("\nvisitor %d is in %s",i,vehicle);
		}
       
    }
    // VALET THREADS ARE CREATED HERE
    //valetThread=new Thread[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[20];
        sprintf(name,"valetThread%d",i);
        valetThread=new Thread(name);
        valetThread->Fork((VoidFunctionPtr)valet,i);
    }

	// TICKET TAKER THREADS ARE CREATED
    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
        name=new char[20];
        sprintf(name,"ticketThread%d",i);
        ticketTakerThreadPtr=new Thread(name);
        ticketTakerThreadPtr->Fork((VoidFunctionPtr)ticketTakerFunction,i);
    }


	//LIMO DRIVER THREADS ARE CREATED HERE
    for(int i=totalNumberOfDrivers;i<totalCountOfLimoDrivers+totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"limoDriverThread%d",i);
		//printf("\n creating limo-driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)limoDriver,i);
    }

	//VISITOR THREADS ARE CREATED HERE
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
        name=new char[20];
        sprintf(name,"visitorThread%d",i);
        visitorThreadPtr=new Thread(name);
        visitorThreadPtr->Fork((VoidFunctionPtr)visitorFunction,i);

    }
    //DRIVER THREADS ARE CREATED HERE
    for(int i=0;i<totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"driverThread%d",i);
		//printf("\n creating driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)driver,i);

    }
	valetManagerThreadPtr=new Thread("valetManager");
	valetManagerThreadPtr->Fork((VoidFunctionPtr)valetManager,-10);

	

    printf("\n");

}


void setupTestCase1()
{
	char *name;

    //printf("\n Enter the total number of valets in simulation :");
    //scanf("%d",&totalNumberOfValets);
	totalNumberOfValets=3;

   //printf("\n Enter the total number of cars in this simulation:");
    //scanf("%d",&totalNumberOfCars);
	totalNumberOfCars=1;

    totalNumberOfDrivers=totalNumberOfCars;

    //printf("\n Enter the total number of limousines:");
   //scanf("%d",&totalCountOfLimos);
	totalCountOfLimos=20;
   
    totalCountOfLimoDrivers=totalCountOfLimos;

    //printf("\n Please enter the total number of ticket takers");
    //scanf("%d",&totalNumberOfTicketTakers);
	totalNumberOfTicketTakers=4;


    //-------------------------CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT-----------------------------

	
	exitLimoQueueLengthLock=new Lock("exitLimoQueueLengthLock");
	CV_exitLimoQueueLength=new Condition("CV_exitLimoQueueLength");

	exitCarQueueLengthLock=new Lock("exitCarQueueLengthLock");
	CV_exitCarQueueLength=new Condition("CV_exitCarQueueLength");


    carQueueLengthLock=new Lock("carQueueLengthLock");
    CV_waitingToGetParked=new Condition("CV_waitingToGetParked");

    limoQueueLengthLock=new Lock("limoQueueLengthLock");
    CV_limosWaitingToGetParked=new Condition("CV_limosWaitingToGetParked");


    countOfExitedCarLock=new Lock("countOfExitedCarLock");

    waitingValetCountLock=new Lock("waitingValetCountLock");
    CV_countOfWaitingValets=new Condition("CV_countOfWaitingValets");

    readyToPark=new int[totalNumberOfCars+totalCountOfLimos];
    readyToParkLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_waitingToParkCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

    visitorPleaseGetOut=new int[totalNumberOfCars+totalCountOfLimos];
    visitorPleaseGetOutLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorPleaseGetOut=new Condition*[totalNumberOfCars+totalCountOfLimos];

    totalNumberOfVisitors=new int[totalNumberOfCars+totalCountOfLimos];
    totalNumberOfVisitorsLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    visitorCount=new int [totalNumberOfCars+totalCountOfLimos];
    visitorCountLock= new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorCount=new Condition*[totalNumberOfCars+totalCountOfLimos];

    carParked=new int[totalNumberOfCars+totalCountOfLimos];
    carParkedLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    parkingToken=new int[totalNumberOfCars+totalCountOfLimos];
    parkingTokenLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_parkingToken=new Condition*[totalNumberOfCars+totalCountOfLimos];

    valetReadyWithCar=new int[totalNumberOfCars+totalCountOfLimos];
    valetReadyWithCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_valetReadyWithCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	driverStatus=new int[totalNumberOfCars+totalCountOfLimos];	
	driverStatusLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_driverStatus=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorsReadyToGoHome=new int[totalNumberOfCars+totalCountOfLimos];
	visitorsReadyToGoHomeLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorReadyToGoHome=new Condition*[totalNumberOfCars+totalCountOfLimos];

	ticketTakerLock=new Lock("ticketTakerLock");

	tipsForThisCar=new int[totalNumberOfCars+totalCountOfLimos];
	tipsForThisCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_tipsForThisCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	carKeysReady=new int[totalNumberOfCars+totalCountOfLimos];	
	carKeysReadyLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carKeysReady=new Condition*[totalNumberOfCars+totalCountOfLimos];	

	carReadyToGo=new int[totalNumberOfCars+totalCountOfLimos];
	carReadyToGoLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carReadyToGo=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorPleaseGetIn=new int[totalNumberOfCars+totalCountOfLimos];
	visitorPleaseGetInLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorPleaseGetIn=new Condition*[totalNumberOfCars+totalCountOfLimos];




    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        readyToPark[i]=0;
        name=new char[50];
        sprintf(name,"readyToParkLock%d",i);
        readyToParkLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_waitingToParkCar%d",i);
        CV_waitingToParkCar[i]=new Condition(name);

        visitorPleaseGetOut[i]=0;
        name=new char[50];
        sprintf(name,"visitorPleaseGetOutLock%d",i);
        visitorPleaseGetOutLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorPleaseGetOut%d",i);
        CV_visitorPleaseGetOut[i]=new Condition(name);

        totalNumberOfVisitors[i]=rand()%5;
		if(i>=totalNumberOfCars)
		{
			totalNumberOfVisitors[i]++;
		}
        grandTotalOfVisitors=grandTotalOfVisitors+totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"totalNumberOfVisitors%d",i);
        totalNumberOfVisitorsLock[i]=new Lock(name);
   
        visitorCount[i]=totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"visitorCountLock%d",i);
        visitorCountLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorCount%d",i);
        CV_visitorCount[i]=new Condition(name);
       
        carParked[i]=-1;
        name=new char[50];
        sprintf(name,"carParkedLock%d",i);
        carParkedLock[i]=new Lock(name);

        parkingToken[i]=-1;
        name=new char[50];
        sprintf(name,"parkingTokenLock%d",i);
        parkingTokenLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_parkingToken%d",i);
		CV_parkingToken[i]=new Condition(name);

        valetReadyWithCar[i]=0;
        name=new char[50];
        sprintf(name,"valetReadyWithCarLock%d",i);
        valetReadyWithCarLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_valetReadyWithCar%d",i);
        CV_valetReadyWithCar[i]=new Condition(name);

		driverStatus[i]=0;
		name=new char[50];
		sprintf(name,"driverStatusLock%d",i);
		driverStatusLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_driverStatus%d",i);
		CV_driverStatus[i]=new Condition(name);

		visitorsReadyToGoHome[i]=0;
		name=new char[50];
		sprintf(name,"visitorsReadyToGoHomeLock%d",i);
		visitorsReadyToGoHomeLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorReadyToGoHome%d",i);
		CV_visitorReadyToGoHome[i]=new Condition(name);

		tipsForThisCar[i]=0;
		name=new char[50];
		sprintf(name,"tipsForThisCarLock%d",i);
		tipsForThisCarLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_tipsForThisCar%d",i);
		CV_tipsForThisCar[i]=new Condition(name);	
		
		carKeysReady[i]=0;
		name=new char[50];
		sprintf(name,"carKeysReadyLock%d",i);
		carKeysReadyLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carKeysReady%d",i);
		CV_carKeysReady[i]=new Condition(name);	


		carReadyToGo[i]=0;
		name=new char[50];
		sprintf(name,"carReadyToGo%d",i);
		carReadyToGoLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carReadyToGo%d",i);
		CV_carReadyToGo[i]=new Condition(name);

	
		visitorPleaseGetIn[i]=0;
		name=new char[50];
		sprintf(name,"visitorPleaseGetInLock%d",i);
		visitorPleaseGetInLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorPleaseGetIn%d",i);
		CV_visitorPleaseGetIn[i]=new Condition(name);



    }
    //--------------------END OF CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT--------------------


	CV_waitingForValetManager=new Condition*[totalNumberOfValets];

	CV_waitingInBackRoom=new Condition*[totalNumberOfValets];

	valetManagerWaitingLock=new Lock("valetManagerWaitingLock");
	CV_valetManagerWaiting=new Condition("CV_valetManagerWaiting");

    valetStatus=new int[totalNumberOfValets];
    valetStatusLock=new Lock*[totalNumberOfValets];
    CV_valetWaitingForDriver=new Condition*[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[50];
        valetStatus[i]=-5;
        sprintf(name,"valetStatusLock%d",i);
        valetStatusLock[i]=new Lock(name);
        //delete(name);
        name=new char[50];
        sprintf(name,"CV_valetWaitingForDriver%d",i);
        CV_valetWaitingForDriver[i]=new Condition(name);
        //delete(name);
		
		name=new char[50];
		sprintf(name,"CV_waitingForValetManager%d",i);
		CV_waitingForValetManager[i]=new Condition(name);

		name=new char[50];
		sprintf(name,"CV_waitingInBackRoom%d",i);
		CV_waitingInBackRoom[i]=new Condition(name);
    }
    //------------------ END OF CODE SEGMENT TO SET UP VALET------------------------------

    //----------------------------- CODE SEGMENT TO SET UP THE TICKET TAKERS ----------------
	
    //printf("\n The Total number of ticket takers in this simulation is %d",totalNumberOfTicketTakers);
   
    CV_noTicketTakers=new Condition("CV_noTicketTakers");
    totalNumberOfActiveTicketTakersLock=new Lock("totalNumberOfActiveTicketTakers");
    ticketTakerQueueLength=new int[totalNumberOfTicketTakers];
   
    ticketTakerQueueLengthLock=new Lock*[totalNumberOfTicketTakers];
   
    CV_waitingTicketTaker=new Condition*[totalNumberOfTicketTakers];

    CV_ticketTakerQueueLength=new Condition*[totalNumberOfTicketTakers];
    visitorMuseumEntryStatus=new int[totalNumberOfTicketTakers];
    visitorMuseumEntryStatusLock=new Lock*[totalNumberOfTicketTakers];
    CV_visitorMuseumEntryStatus=new Condition*[totalNumberOfTicketTakers];

    totalNumberOfVisitorsAcceptedLock=new Lock("totalNumberOfVisitorsAcceptedLock");
    if(totalNumberOfDrivers==0 && totalCountOfLimoDrivers==0)
    {
        totalNumberOfVisitorsAccepted=-1;
    }

    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
       
        ticketTakerQueueLength[i]=0; //SETTING THE INTIAL VALUE TO BE ZERO
        visitorMuseumEntryStatus[i]=0;
       
        name=new char[50];
        sprintf(name,"visitorMuseumEntryStatusLock%d",i);
        visitorMuseumEntryStatusLock[i]=new Lock(name);

        name=new char[50];
        sprintf(name,"CV_visitorMuseumEntryStatus%d",i);
        CV_visitorMuseumEntryStatus[i]=new Condition(name);

        name=new char[50];
        sprintf(name,"ticketTakerQueueLengthLock%d",i);
        ticketTakerQueueLengthLock[i]=new Lock(name);
       
        name=new char[50];
        sprintf(name,"CV_waitingTicketTaker%d",i);
        CV_waitingTicketTaker[i]=new Condition(name);
       
        name=new char[50];
        sprintf(name,"CV_ticketTakerQueueLength%d",i);
        CV_ticketTakerQueueLength[i]=new Condition(name);
       
    }
    //-------------------END OF CODE SEGMENT TO SET UP THE TICKET TAKERS -------------------------------
   


}


void TestCase1()
{

	char *name;
	printf("\n ********************** TEST CASE 1 STARTING UP **************************************");
	printf("\n ********As long as limousines keep arriving, regular cars never get parked***********");
    
	setupTestCase1();

	 for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        printCarDetails(i);
    }
    for(int i=0;i<totalNumberOfValets;i++)
    {
        printValetDetails(i);
    }
    printf("\n");
    visitor=new int[grandTotalOfVisitors];
    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        static int k=0;
        if(totalNumberOfVisitors[i]!=0)
        {
                for(int j=0;j<totalNumberOfVisitors[i];j++)
                {
                    visitor[k]=i;   
                    k++;
                }
        }
    }
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
		char *vehicle=new char[10];
		if(visitor[i]<totalNumberOfCars)
		{
			sprintf(vehicle,"car%d",visitor[i]);
			printf("\nvisitor %d is in  %s",i,vehicle);
		}
		else
		{
			sprintf(vehicle,"Limousine%d",visitor[i]);
			printf("\nvisitor %d is in %s",i,vehicle);
		}
       
    }
    // VALET THREADS ARE CREATED HERE
    //valetThread=new Thread[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[20];
        sprintf(name,"valetThread%d",i);
        valetThread=new Thread(name);
        valetThread->Fork((VoidFunctionPtr)valet,i);
    }

	// TICKET TAKER THREADS ARE CREATED
    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
        name=new char[20];
        sprintf(name,"ticketThread%d",i);
        ticketTakerThreadPtr=new Thread(name);
        ticketTakerThreadPtr->Fork((VoidFunctionPtr)ticketTakerFunction,i);
    }


	//LIMO DRIVER THREADS ARE CREATED HERE
    for(int i=totalNumberOfDrivers;i<totalCountOfLimoDrivers+totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"limoDriverThread%d",i);
		//printf("\n creating limo-driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)limoDriver,i);
    }

	//VISITOR THREADS ARE CREATED HERE
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
        name=new char[20];
        sprintf(name,"visitorThread%d",i);
        visitorThreadPtr=new Thread(name);
        visitorThreadPtr->Fork((VoidFunctionPtr)visitorFunction,i);

    }
    //DRIVER THREADS ARE CREATED HERE
    for(int i=0;i<totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"driverThread%d",i);
		//printf("\n creating driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)driver,i);

    }
	valetManagerThreadPtr=new Thread("valetManager");
	valetManagerThreadPtr->Fork((VoidFunctionPtr)valetManager,-10);

	

    printf("\n");


}

void setupTestCase2()
{
	char *name;

    //printf("\n Enter the total number of valets in simulation :");
    //scanf("%d",&totalNumberOfValets);
	totalNumberOfValets=0;

   //printf("\n Enter the total number of cars in this simulation:");
    //scanf("%d",&totalNumberOfCars);
	totalNumberOfCars=10;

    totalNumberOfDrivers=totalNumberOfCars;

    //printf("\n Enter the total number of limousines:");
   //scanf("%d",&totalCountOfLimos);
	totalCountOfLimos=10;
   
    totalCountOfLimoDrivers=totalCountOfLimos;

    //printf("\n Please enter the total number of ticket takers");
    //scanf("%d",&totalNumberOfTicketTakers);
	totalNumberOfTicketTakers=4;


    //-------------------------CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT-----------------------------

	
	exitLimoQueueLengthLock=new Lock("exitLimoQueueLengthLock");
	CV_exitLimoQueueLength=new Condition("CV_exitLimoQueueLength");

	exitCarQueueLengthLock=new Lock("exitCarQueueLengthLock");
	CV_exitCarQueueLength=new Condition("CV_exitCarQueueLength");


    carQueueLengthLock=new Lock("carQueueLengthLock");
    CV_waitingToGetParked=new Condition("CV_waitingToGetParked");

    limoQueueLengthLock=new Lock("limoQueueLengthLock");
    CV_limosWaitingToGetParked=new Condition("CV_limosWaitingToGetParked");


    countOfExitedCarLock=new Lock("countOfExitedCarLock");

    waitingValetCountLock=new Lock("waitingValetCountLock");
    CV_countOfWaitingValets=new Condition("CV_countOfWaitingValets");

    readyToPark=new int[totalNumberOfCars+totalCountOfLimos];
    readyToParkLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_waitingToParkCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

    visitorPleaseGetOut=new int[totalNumberOfCars+totalCountOfLimos];
    visitorPleaseGetOutLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorPleaseGetOut=new Condition*[totalNumberOfCars+totalCountOfLimos];

    totalNumberOfVisitors=new int[totalNumberOfCars+totalCountOfLimos];
    totalNumberOfVisitorsLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    visitorCount=new int [totalNumberOfCars+totalCountOfLimos];
    visitorCountLock= new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorCount=new Condition*[totalNumberOfCars+totalCountOfLimos];

    carParked=new int[totalNumberOfCars+totalCountOfLimos];
    carParkedLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    parkingToken=new int[totalNumberOfCars+totalCountOfLimos];
    parkingTokenLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_parkingToken=new Condition*[totalNumberOfCars+totalCountOfLimos];

    valetReadyWithCar=new int[totalNumberOfCars+totalCountOfLimos];
    valetReadyWithCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_valetReadyWithCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	driverStatus=new int[totalNumberOfCars+totalCountOfLimos];	
	driverStatusLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_driverStatus=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorsReadyToGoHome=new int[totalNumberOfCars+totalCountOfLimos];
	visitorsReadyToGoHomeLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorReadyToGoHome=new Condition*[totalNumberOfCars+totalCountOfLimos];

	ticketTakerLock=new Lock("ticketTakerLock");

	tipsForThisCar=new int[totalNumberOfCars+totalCountOfLimos];
	tipsForThisCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_tipsForThisCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	carKeysReady=new int[totalNumberOfCars+totalCountOfLimos];	
	carKeysReadyLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carKeysReady=new Condition*[totalNumberOfCars+totalCountOfLimos];	

	carReadyToGo=new int[totalNumberOfCars+totalCountOfLimos];
	carReadyToGoLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carReadyToGo=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorPleaseGetIn=new int[totalNumberOfCars+totalCountOfLimos];
	visitorPleaseGetInLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorPleaseGetIn=new Condition*[totalNumberOfCars+totalCountOfLimos];




    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        readyToPark[i]=0;
        name=new char[50];
        sprintf(name,"readyToParkLock%d",i);
        readyToParkLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_waitingToParkCar%d",i);
        CV_waitingToParkCar[i]=new Condition(name);

        visitorPleaseGetOut[i]=0;
        name=new char[50];
        sprintf(name,"visitorPleaseGetOutLock%d",i);
        visitorPleaseGetOutLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorPleaseGetOut%d",i);
        CV_visitorPleaseGetOut[i]=new Condition(name);

        totalNumberOfVisitors[i]=rand()%5;
		if(i>=totalNumberOfCars)
		{
			totalNumberOfVisitors[i]++;
		}
        grandTotalOfVisitors=grandTotalOfVisitors+totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"totalNumberOfVisitors%d",i);
        totalNumberOfVisitorsLock[i]=new Lock(name);
   
        visitorCount[i]=totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"visitorCountLock%d",i);
        visitorCountLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorCount%d",i);
        CV_visitorCount[i]=new Condition(name);
       
        carParked[i]=-1;
        name=new char[50];
        sprintf(name,"carParkedLock%d",i);
        carParkedLock[i]=new Lock(name);

        parkingToken[i]=-1;
        name=new char[50];
        sprintf(name,"parkingTokenLock%d",i);
        parkingTokenLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_parkingToken%d",i);
		CV_parkingToken[i]=new Condition(name);

        valetReadyWithCar[i]=0;
        name=new char[50];
        sprintf(name,"valetReadyWithCarLock%d",i);
        valetReadyWithCarLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_valetReadyWithCar%d",i);
        CV_valetReadyWithCar[i]=new Condition(name);

		driverStatus[i]=0;
		name=new char[50];
		sprintf(name,"driverStatusLock%d",i);
		driverStatusLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_driverStatus%d",i);
		CV_driverStatus[i]=new Condition(name);

		visitorsReadyToGoHome[i]=0;
		name=new char[50];
		sprintf(name,"visitorsReadyToGoHomeLock%d",i);
		visitorsReadyToGoHomeLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorReadyToGoHome%d",i);
		CV_visitorReadyToGoHome[i]=new Condition(name);

		tipsForThisCar[i]=0;
		name=new char[50];
		sprintf(name,"tipsForThisCarLock%d",i);
		tipsForThisCarLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_tipsForThisCar%d",i);
		CV_tipsForThisCar[i]=new Condition(name);	
		
		carKeysReady[i]=0;
		name=new char[50];
		sprintf(name,"carKeysReadyLock%d",i);
		carKeysReadyLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carKeysReady%d",i);
		CV_carKeysReady[i]=new Condition(name);	


		carReadyToGo[i]=0;
		name=new char[50];
		sprintf(name,"carReadyToGo%d",i);
		carReadyToGoLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carReadyToGo%d",i);
		CV_carReadyToGo[i]=new Condition(name);

	
		visitorPleaseGetIn[i]=0;
		name=new char[50];
		sprintf(name,"visitorPleaseGetInLock%d",i);
		visitorPleaseGetInLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorPleaseGetIn%d",i);
		CV_visitorPleaseGetIn[i]=new Condition(name);



    }
    //--------------------END OF CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT--------------------


	CV_waitingForValetManager=new Condition*[totalNumberOfValets];

	CV_waitingInBackRoom=new Condition*[totalNumberOfValets];

	valetManagerWaitingLock=new Lock("valetManagerWaitingLock");
	CV_valetManagerWaiting=new Condition("CV_valetManagerWaiting");

    valetStatus=new int[totalNumberOfValets];
    valetStatusLock=new Lock*[totalNumberOfValets];
    CV_valetWaitingForDriver=new Condition*[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[50];
        valetStatus[i]=-5;
        sprintf(name,"valetStatusLock%d",i);
        valetStatusLock[i]=new Lock(name);
        //delete(name);
        name=new char[50];
        sprintf(name,"CV_valetWaitingForDriver%d",i);
        CV_valetWaitingForDriver[i]=new Condition(name);
        //delete(name);
		
		name=new char[50];
		sprintf(name,"CV_waitingForValetManager%d",i);
		CV_waitingForValetManager[i]=new Condition(name);

		name=new char[50];
		sprintf(name,"CV_waitingInBackRoom%d",i);
		CV_waitingInBackRoom[i]=new Condition(name);
    }
    //------------------ END OF CODE SEGMENT TO SET UP VALET------------------------------

    //----------------------------- CODE SEGMENT TO SET UP THE TICKET TAKERS ----------------
	
    //printf("\n The Total number of ticket takers in this simulation is %d",totalNumberOfTicketTakers);
   
    CV_noTicketTakers=new Condition("CV_noTicketTakers");
    totalNumberOfActiveTicketTakersLock=new Lock("totalNumberOfActiveTicketTakers");
    ticketTakerQueueLength=new int[totalNumberOfTicketTakers];
   
    ticketTakerQueueLengthLock=new Lock*[totalNumberOfTicketTakers];
   
    CV_waitingTicketTaker=new Condition*[totalNumberOfTicketTakers];

    CV_ticketTakerQueueLength=new Condition*[totalNumberOfTicketTakers];
    visitorMuseumEntryStatus=new int[totalNumberOfTicketTakers];
    visitorMuseumEntryStatusLock=new Lock*[totalNumberOfTicketTakers];
    CV_visitorMuseumEntryStatus=new Condition*[totalNumberOfTicketTakers];

    totalNumberOfVisitorsAcceptedLock=new Lock("totalNumberOfVisitorsAcceptedLock");
    if(totalNumberOfDrivers==0 && totalCountOfLimoDrivers==0)
    {
        totalNumberOfVisitorsAccepted=-1;
    }

    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
       
        ticketTakerQueueLength[i]=0; //SETTING THE INTIAL VALUE TO BE ZERO
        visitorMuseumEntryStatus[i]=0;
       
        name=new char[50];
        sprintf(name,"visitorMuseumEntryStatusLock%d",i);
        visitorMuseumEntryStatusLock[i]=new Lock(name);

        name=new char[50];
        sprintf(name,"CV_visitorMuseumEntryStatus%d",i);
        CV_visitorMuseumEntryStatus[i]=new Condition(name);

        name=new char[50];
        sprintf(name,"ticketTakerQueueLengthLock%d",i);
        ticketTakerQueueLengthLock[i]=new Lock(name);
       
        name=new char[50];
        sprintf(name,"CV_waitingTicketTaker%d",i);
        CV_waitingTicketTaker[i]=new Condition(name);
       
        name=new char[50];
        sprintf(name,"CV_ticketTakerQueueLength%d",i);
        CV_ticketTakerQueueLength[i]=new Condition(name);
       
    }
    //-------------------END OF CODE SEGMENT TO SET UP THE TICKET TAKERS -------------------------------
   


}


void TestCase2()
{

	char *name;
	printf("\n ********************** TEST CASE 2 STARTING UP **************************************");
	printf("\n *****************If no Valets are available, vehicles don't get parked***************");
    
	setupTestCase2();

	 for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        printCarDetails(i);
    }
    for(int i=0;i<totalNumberOfValets;i++)
    {
        printValetDetails(i);
    }
    printf("\n");
    visitor=new int[grandTotalOfVisitors];
    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        static int k=0;
        if(totalNumberOfVisitors[i]!=0)
        {
                for(int j=0;j<totalNumberOfVisitors[i];j++)
                {
                    visitor[k]=i;   
                    k++;
                }
        }
    }
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
		char *vehicle=new char[10];
		if(visitor[i]<totalNumberOfCars)
		{
			sprintf(vehicle,"car%d",visitor[i]);
			printf("\nvisitor %d is in  %s",i,vehicle);
		}
		else
		{
			sprintf(vehicle,"Limousine%d",visitor[i]);
			printf("\nvisitor %d is in %s",i,vehicle);
		}
       
    }
    // VALET THREADS ARE CREATED HERE
    //valetThread=new Thread[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[20];
        sprintf(name,"valetThread%d",i);
        valetThread=new Thread(name);
        valetThread->Fork((VoidFunctionPtr)valet,i);
    }

	// TICKET TAKER THREADS ARE CREATED
    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
        name=new char[20];
        sprintf(name,"ticketThread%d",i);
        ticketTakerThreadPtr=new Thread(name);
        ticketTakerThreadPtr->Fork((VoidFunctionPtr)ticketTakerFunction,i);
    }


	//LIMO DRIVER THREADS ARE CREATED HERE
    for(int i=totalNumberOfDrivers;i<totalCountOfLimoDrivers+totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"limoDriverThread%d",i);
		//printf("\n creating limo-driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)limoDriver,i);
    }

	//VISITOR THREADS ARE CREATED HERE
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
        name=new char[20];
        sprintf(name,"visitorThread%d",i);
        visitorThreadPtr=new Thread(name);
        visitorThreadPtr->Fork((VoidFunctionPtr)visitorFunction,i);

    }
    //DRIVER THREADS ARE CREATED HERE
    for(int i=0;i<totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"driverThread%d",i);
		//printf("\n creating driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)driver,i);

    }
	valetManagerThreadPtr=new Thread("valetManager");
	valetManagerThreadPtr->Fork((VoidFunctionPtr)valetManager,-10);

	

    printf("\n");


}


void setupTestCase3()
{
	char *name;

    //printf("\n Enter the total number of valets in simulation :");
    //scanf("%d",&totalNumberOfValets);
	totalNumberOfValets=10;

   //printf("\n Enter the total number of cars in this simulation:");
    //scanf("%d",&totalNumberOfCars);
	totalNumberOfCars=0;

    totalNumberOfDrivers=totalNumberOfCars;

    //printf("\n Enter the total number of limousines:");
   //scanf("%d",&totalCountOfLimos);
	totalCountOfLimos=0;
   
    totalCountOfLimoDrivers=totalCountOfLimos;

    //printf("\n Please enter the total number of ticket takers");
    //scanf("%d",&totalNumberOfTicketTakers);
	totalNumberOfTicketTakers=0;


    //-------------------------CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT-----------------------------

	
	exitLimoQueueLengthLock=new Lock("exitLimoQueueLengthLock");
	CV_exitLimoQueueLength=new Condition("CV_exitLimoQueueLength");

	exitCarQueueLengthLock=new Lock("exitCarQueueLengthLock");
	CV_exitCarQueueLength=new Condition("CV_exitCarQueueLength");


    carQueueLengthLock=new Lock("carQueueLengthLock");
    CV_waitingToGetParked=new Condition("CV_waitingToGetParked");

    limoQueueLengthLock=new Lock("limoQueueLengthLock");
    CV_limosWaitingToGetParked=new Condition("CV_limosWaitingToGetParked");


    countOfExitedCarLock=new Lock("countOfExitedCarLock");

    waitingValetCountLock=new Lock("waitingValetCountLock");
    CV_countOfWaitingValets=new Condition("CV_countOfWaitingValets");

    readyToPark=new int[totalNumberOfCars+totalCountOfLimos];
    readyToParkLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_waitingToParkCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

    visitorPleaseGetOut=new int[totalNumberOfCars+totalCountOfLimos];
    visitorPleaseGetOutLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorPleaseGetOut=new Condition*[totalNumberOfCars+totalCountOfLimos];

    totalNumberOfVisitors=new int[totalNumberOfCars+totalCountOfLimos];
    totalNumberOfVisitorsLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    visitorCount=new int [totalNumberOfCars+totalCountOfLimos];
    visitorCountLock= new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorCount=new Condition*[totalNumberOfCars+totalCountOfLimos];

    carParked=new int[totalNumberOfCars+totalCountOfLimos];
    carParkedLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    parkingToken=new int[totalNumberOfCars+totalCountOfLimos];
    parkingTokenLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_parkingToken=new Condition*[totalNumberOfCars+totalCountOfLimos];

    valetReadyWithCar=new int[totalNumberOfCars+totalCountOfLimos];
    valetReadyWithCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_valetReadyWithCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	driverStatus=new int[totalNumberOfCars+totalCountOfLimos];	
	driverStatusLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_driverStatus=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorsReadyToGoHome=new int[totalNumberOfCars+totalCountOfLimos];
	visitorsReadyToGoHomeLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorReadyToGoHome=new Condition*[totalNumberOfCars+totalCountOfLimos];

	ticketTakerLock=new Lock("ticketTakerLock");

	tipsForThisCar=new int[totalNumberOfCars+totalCountOfLimos];
	tipsForThisCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_tipsForThisCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	carKeysReady=new int[totalNumberOfCars+totalCountOfLimos];	
	carKeysReadyLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carKeysReady=new Condition*[totalNumberOfCars+totalCountOfLimos];	

	carReadyToGo=new int[totalNumberOfCars+totalCountOfLimos];
	carReadyToGoLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carReadyToGo=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorPleaseGetIn=new int[totalNumberOfCars+totalCountOfLimos];
	visitorPleaseGetInLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorPleaseGetIn=new Condition*[totalNumberOfCars+totalCountOfLimos];




    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        readyToPark[i]=0;
        name=new char[50];
        sprintf(name,"readyToParkLock%d",i);
        readyToParkLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_waitingToParkCar%d",i);
        CV_waitingToParkCar[i]=new Condition(name);

        visitorPleaseGetOut[i]=0;
        name=new char[50];
        sprintf(name,"visitorPleaseGetOutLock%d",i);
        visitorPleaseGetOutLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorPleaseGetOut%d",i);
        CV_visitorPleaseGetOut[i]=new Condition(name);

        totalNumberOfVisitors[i]=rand()%5;
		if(i>=totalNumberOfCars)
		{
			totalNumberOfVisitors[i]++;
		}
        grandTotalOfVisitors=grandTotalOfVisitors+totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"totalNumberOfVisitors%d",i);
        totalNumberOfVisitorsLock[i]=new Lock(name);
   
        visitorCount[i]=totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"visitorCountLock%d",i);
        visitorCountLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorCount%d",i);
        CV_visitorCount[i]=new Condition(name);
       
        carParked[i]=-1;
        name=new char[50];
        sprintf(name,"carParkedLock%d",i);
        carParkedLock[i]=new Lock(name);

        parkingToken[i]=-1;
        name=new char[50];
        sprintf(name,"parkingTokenLock%d",i);
        parkingTokenLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_parkingToken%d",i);
		CV_parkingToken[i]=new Condition(name);

        valetReadyWithCar[i]=0;
        name=new char[50];
        sprintf(name,"valetReadyWithCarLock%d",i);
        valetReadyWithCarLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_valetReadyWithCar%d",i);
        CV_valetReadyWithCar[i]=new Condition(name);

		driverStatus[i]=0;
		name=new char[50];
		sprintf(name,"driverStatusLock%d",i);
		driverStatusLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_driverStatus%d",i);
		CV_driverStatus[i]=new Condition(name);

		visitorsReadyToGoHome[i]=0;
		name=new char[50];
		sprintf(name,"visitorsReadyToGoHomeLock%d",i);
		visitorsReadyToGoHomeLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorReadyToGoHome%d",i);
		CV_visitorReadyToGoHome[i]=new Condition(name);

		tipsForThisCar[i]=0;
		name=new char[50];
		sprintf(name,"tipsForThisCarLock%d",i);
		tipsForThisCarLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_tipsForThisCar%d",i);
		CV_tipsForThisCar[i]=new Condition(name);	
		
		carKeysReady[i]=0;
		name=new char[50];
		sprintf(name,"carKeysReadyLock%d",i);
		carKeysReadyLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carKeysReady%d",i);
		CV_carKeysReady[i]=new Condition(name);	


		carReadyToGo[i]=0;
		name=new char[50];
		sprintf(name,"carReadyToGo%d",i);
		carReadyToGoLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carReadyToGo%d",i);
		CV_carReadyToGo[i]=new Condition(name);

	
		visitorPleaseGetIn[i]=0;
		name=new char[50];
		sprintf(name,"visitorPleaseGetInLock%d",i);
		visitorPleaseGetInLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorPleaseGetIn%d",i);
		CV_visitorPleaseGetIn[i]=new Condition(name);



    }
    //--------------------END OF CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT--------------------


	CV_waitingForValetManager=new Condition*[totalNumberOfValets];

	CV_waitingInBackRoom=new Condition*[totalNumberOfValets];

	valetManagerWaitingLock=new Lock("valetManagerWaitingLock");
	CV_valetManagerWaiting=new Condition("CV_valetManagerWaiting");

    valetStatus=new int[totalNumberOfValets];
    valetStatusLock=new Lock*[totalNumberOfValets];
    CV_valetWaitingForDriver=new Condition*[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[50];
        valetStatus[i]=-5;
        sprintf(name,"valetStatusLock%d",i);
        valetStatusLock[i]=new Lock(name);
        //delete(name);
        name=new char[50];
        sprintf(name,"CV_valetWaitingForDriver%d",i);
        CV_valetWaitingForDriver[i]=new Condition(name);
        //delete(name);
		
		name=new char[50];
		sprintf(name,"CV_waitingForValetManager%d",i);
		CV_waitingForValetManager[i]=new Condition(name);

		name=new char[50];
		sprintf(name,"CV_waitingInBackRoom%d",i);
		CV_waitingInBackRoom[i]=new Condition(name);
    }
    //------------------ END OF CODE SEGMENT TO SET UP VALET------------------------------

    //----------------------------- CODE SEGMENT TO SET UP THE TICKET TAKERS ----------------
	
    //printf("\n The Total number of ticket takers in this simulation is %d",totalNumberOfTicketTakers);
   
    CV_noTicketTakers=new Condition("CV_noTicketTakers");
    totalNumberOfActiveTicketTakersLock=new Lock("totalNumberOfActiveTicketTakers");
    ticketTakerQueueLength=new int[totalNumberOfTicketTakers];
   
    ticketTakerQueueLengthLock=new Lock*[totalNumberOfTicketTakers];
   
    CV_waitingTicketTaker=new Condition*[totalNumberOfTicketTakers];

    CV_ticketTakerQueueLength=new Condition*[totalNumberOfTicketTakers];
    visitorMuseumEntryStatus=new int[totalNumberOfTicketTakers];
    visitorMuseumEntryStatusLock=new Lock*[totalNumberOfTicketTakers];
    CV_visitorMuseumEntryStatus=new Condition*[totalNumberOfTicketTakers];

    totalNumberOfVisitorsAcceptedLock=new Lock("totalNumberOfVisitorsAcceptedLock");
    if(totalNumberOfDrivers==0 && totalCountOfLimoDrivers==0)
    {
        totalNumberOfVisitorsAccepted=-1;
    }

    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
       
        ticketTakerQueueLength[i]=0; //SETTING THE INTIAL VALUE TO BE ZERO
        visitorMuseumEntryStatus[i]=0;
       
        name=new char[50];
        sprintf(name,"visitorMuseumEntryStatusLock%d",i);
        visitorMuseumEntryStatusLock[i]=new Lock(name);

        name=new char[50];
        sprintf(name,"CV_visitorMuseumEntryStatus%d",i);
        CV_visitorMuseumEntryStatus[i]=new Condition(name);

        name=new char[50];
        sprintf(name,"ticketTakerQueueLengthLock%d",i);
        ticketTakerQueueLengthLock[i]=new Lock(name);
       
        name=new char[50];
        sprintf(name,"CV_waitingTicketTaker%d",i);
        CV_waitingTicketTaker[i]=new Condition(name);
       
        name=new char[50];
        sprintf(name,"CV_ticketTakerQueueLength%d",i);
        CV_ticketTakerQueueLength[i]=new Condition(name);
       
    }
    //-------------------END OF CODE SEGMENT TO SET UP THE TICKET TAKERS -------------------------------
   


}


void TestCase3()
{

	char *name;
	printf("\n ********************** TEST CASE 3 STARTING UP **************************************");
	printf("\n **********************The Valet Manager performs their job properly******************");
    
	setupTestCase3();

	 for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        printCarDetails(i);
    }
    for(int i=0;i<totalNumberOfValets;i++)
    {
        printValetDetails(i);
    }
    printf("\n");
    visitor=new int[grandTotalOfVisitors];
    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        static int k=0;
        if(totalNumberOfVisitors[i]!=0)
        {
                for(int j=0;j<totalNumberOfVisitors[i];j++)
                {
                    visitor[k]=i;   
                    k++;
                }
        }
    }
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
		char *vehicle=new char[10];
		if(visitor[i]<totalNumberOfCars)
		{
			sprintf(vehicle,"car%d",visitor[i]);
			printf("\nvisitor %d is in  %s",i,vehicle);
		}
		else
		{
			sprintf(vehicle,"Limousine%d",visitor[i]);
			printf("\nvisitor %d is in %s",i,vehicle);
		}
       
    }
    // VALET THREADS ARE CREATED HERE
    //valetThread=new Thread[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[20];
        sprintf(name,"valetThread%d",i);
        valetThread=new Thread(name);
        valetThread->Fork((VoidFunctionPtr)valet,i);
    }

	// TICKET TAKER THREADS ARE CREATED
    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
        name=new char[20];
        sprintf(name,"ticketThread%d",i);
        ticketTakerThreadPtr=new Thread(name);
        ticketTakerThreadPtr->Fork((VoidFunctionPtr)ticketTakerFunction,i);
    }


	//LIMO DRIVER THREADS ARE CREATED HERE
    for(int i=totalNumberOfDrivers;i<totalCountOfLimoDrivers+totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"limoDriverThread%d",i);
		//printf("\n creating limo-driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)limoDriver,i);
    }

	//VISITOR THREADS ARE CREATED HERE
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
        name=new char[20];
        sprintf(name,"visitorThread%d",i);
        visitorThreadPtr=new Thread(name);
        visitorThreadPtr->Fork((VoidFunctionPtr)visitorFunction,i);

    }
    //DRIVER THREADS ARE CREATED HERE
    for(int i=0;i<totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"driverThread%d",i);
		//printf("\n creating driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)driver,i);

    }
	valetManagerThreadPtr=new Thread("valetManager");
	valetManagerThreadPtr->Fork((VoidFunctionPtr)valetManager,-10);

	

    printf("\n");


}

void setupTestCase4()
{
	char *name;

    //printf("\n Enter the total number of valets in simulation :");
    //scanf("%d",&totalNumberOfValets);
	totalNumberOfValets=5;

   //printf("\n Enter the total number of cars in this simulation:");
    //scanf("%d",&totalNumberOfCars);
	totalNumberOfCars=5;

    totalNumberOfDrivers=totalNumberOfCars;

    //printf("\n Enter the total number of limousines:");
   //scanf("%d",&totalCountOfLimos);
	totalCountOfLimos=5;
   
    totalCountOfLimoDrivers=totalCountOfLimos;

    //printf("\n Please enter the total number of ticket takers");
    //scanf("%d",&totalNumberOfTicketTakers);
	totalNumberOfTicketTakers=0;


    //-------------------------CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT-----------------------------

	
	exitLimoQueueLengthLock=new Lock("exitLimoQueueLengthLock");
	CV_exitLimoQueueLength=new Condition("CV_exitLimoQueueLength");

	exitCarQueueLengthLock=new Lock("exitCarQueueLengthLock");
	CV_exitCarQueueLength=new Condition("CV_exitCarQueueLength");


    carQueueLengthLock=new Lock("carQueueLengthLock");
    CV_waitingToGetParked=new Condition("CV_waitingToGetParked");

    limoQueueLengthLock=new Lock("limoQueueLengthLock");
    CV_limosWaitingToGetParked=new Condition("CV_limosWaitingToGetParked");


    countOfExitedCarLock=new Lock("countOfExitedCarLock");

    waitingValetCountLock=new Lock("waitingValetCountLock");
    CV_countOfWaitingValets=new Condition("CV_countOfWaitingValets");

    readyToPark=new int[totalNumberOfCars+totalCountOfLimos];
    readyToParkLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_waitingToParkCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

    visitorPleaseGetOut=new int[totalNumberOfCars+totalCountOfLimos];
    visitorPleaseGetOutLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorPleaseGetOut=new Condition*[totalNumberOfCars+totalCountOfLimos];

    totalNumberOfVisitors=new int[totalNumberOfCars+totalCountOfLimos];
    totalNumberOfVisitorsLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    visitorCount=new int [totalNumberOfCars+totalCountOfLimos];
    visitorCountLock= new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_visitorCount=new Condition*[totalNumberOfCars+totalCountOfLimos];

    carParked=new int[totalNumberOfCars+totalCountOfLimos];
    carParkedLock=new Lock*[totalNumberOfCars+totalCountOfLimos];

    parkingToken=new int[totalNumberOfCars+totalCountOfLimos];
    parkingTokenLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_parkingToken=new Condition*[totalNumberOfCars+totalCountOfLimos];

    valetReadyWithCar=new int[totalNumberOfCars+totalCountOfLimos];
    valetReadyWithCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
    CV_valetReadyWithCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	driverStatus=new int[totalNumberOfCars+totalCountOfLimos];	
	driverStatusLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_driverStatus=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorsReadyToGoHome=new int[totalNumberOfCars+totalCountOfLimos];
	visitorsReadyToGoHomeLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorReadyToGoHome=new Condition*[totalNumberOfCars+totalCountOfLimos];

	ticketTakerLock=new Lock("ticketTakerLock");

	tipsForThisCar=new int[totalNumberOfCars+totalCountOfLimos];
	tipsForThisCarLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_tipsForThisCar=new Condition*[totalNumberOfCars+totalCountOfLimos];

	carKeysReady=new int[totalNumberOfCars+totalCountOfLimos];	
	carKeysReadyLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carKeysReady=new Condition*[totalNumberOfCars+totalCountOfLimos];	

	carReadyToGo=new int[totalNumberOfCars+totalCountOfLimos];
	carReadyToGoLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_carReadyToGo=new Condition*[totalNumberOfCars+totalCountOfLimos];

	visitorPleaseGetIn=new int[totalNumberOfCars+totalCountOfLimos];
	visitorPleaseGetInLock=new Lock*[totalNumberOfCars+totalCountOfLimos];
	CV_visitorPleaseGetIn=new Condition*[totalNumberOfCars+totalCountOfLimos];




    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        readyToPark[i]=0;
        name=new char[50];
        sprintf(name,"readyToParkLock%d",i);
        readyToParkLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_waitingToParkCar%d",i);
        CV_waitingToParkCar[i]=new Condition(name);

        visitorPleaseGetOut[i]=0;
        name=new char[50];
        sprintf(name,"visitorPleaseGetOutLock%d",i);
        visitorPleaseGetOutLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorPleaseGetOut%d",i);
        CV_visitorPleaseGetOut[i]=new Condition(name);

        totalNumberOfVisitors[i]=rand()%5;
		if(i>=totalNumberOfCars)
		{
			totalNumberOfVisitors[i]++;
		}
        grandTotalOfVisitors=grandTotalOfVisitors+totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"totalNumberOfVisitors%d",i);
        totalNumberOfVisitorsLock[i]=new Lock(name);
   
        visitorCount[i]=totalNumberOfVisitors[i];
        name=new char[50];
        sprintf(name,"visitorCountLock%d",i);
        visitorCountLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_visitorCount%d",i);
        CV_visitorCount[i]=new Condition(name);
       
        carParked[i]=-1;
        name=new char[50];
        sprintf(name,"carParkedLock%d",i);
        carParkedLock[i]=new Lock(name);

        parkingToken[i]=-1;
        name=new char[50];
        sprintf(name,"parkingTokenLock%d",i);
        parkingTokenLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_parkingToken%d",i);
		CV_parkingToken[i]=new Condition(name);

        valetReadyWithCar[i]=0;
        name=new char[50];
        sprintf(name,"valetReadyWithCarLock%d",i);
        valetReadyWithCarLock[i]=new Lock(name);
        name=new char[50];
        sprintf(name,"CV_valetReadyWithCar%d",i);
        CV_valetReadyWithCar[i]=new Condition(name);

		driverStatus[i]=0;
		name=new char[50];
		sprintf(name,"driverStatusLock%d",i);
		driverStatusLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_driverStatus%d",i);
		CV_driverStatus[i]=new Condition(name);

		visitorsReadyToGoHome[i]=0;
		name=new char[50];
		sprintf(name,"visitorsReadyToGoHomeLock%d",i);
		visitorsReadyToGoHomeLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorReadyToGoHome%d",i);
		CV_visitorReadyToGoHome[i]=new Condition(name);

		tipsForThisCar[i]=0;
		name=new char[50];
		sprintf(name,"tipsForThisCarLock%d",i);
		tipsForThisCarLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_tipsForThisCar%d",i);
		CV_tipsForThisCar[i]=new Condition(name);	
		
		carKeysReady[i]=0;
		name=new char[50];
		sprintf(name,"carKeysReadyLock%d",i);
		carKeysReadyLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carKeysReady%d",i);
		CV_carKeysReady[i]=new Condition(name);	


		carReadyToGo[i]=0;
		name=new char[50];
		sprintf(name,"carReadyToGo%d",i);
		carReadyToGoLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_carReadyToGo%d",i);
		CV_carReadyToGo[i]=new Condition(name);

	
		visitorPleaseGetIn[i]=0;
		name=new char[50];
		sprintf(name,"visitorPleaseGetInLock%d",i);
		visitorPleaseGetInLock[i]=new Lock(name);
		name=new char[50];
		sprintf(name,"CV_visitorPleaseGetIn%d",i);
		CV_visitorPleaseGetIn[i]=new Condition(name);



    }
    //--------------------END OF CODE SEGMENT TO SETUP THE CAR ATTRIBUTES AND THE LOCK AND CONDITION VARIBALES ON IT--------------------


	CV_waitingForValetManager=new Condition*[totalNumberOfValets];

	CV_waitingInBackRoom=new Condition*[totalNumberOfValets];

	valetManagerWaitingLock=new Lock("valetManagerWaitingLock");
	CV_valetManagerWaiting=new Condition("CV_valetManagerWaiting");

    valetStatus=new int[totalNumberOfValets];
    valetStatusLock=new Lock*[totalNumberOfValets];
    CV_valetWaitingForDriver=new Condition*[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[50];
        valetStatus[i]=-5;
        sprintf(name,"valetStatusLock%d",i);
        valetStatusLock[i]=new Lock(name);
        //delete(name);
        name=new char[50];
        sprintf(name,"CV_valetWaitingForDriver%d",i);
        CV_valetWaitingForDriver[i]=new Condition(name);
        //delete(name);
		
		name=new char[50];
		sprintf(name,"CV_waitingForValetManager%d",i);
		CV_waitingForValetManager[i]=new Condition(name);

		name=new char[50];
		sprintf(name,"CV_waitingInBackRoom%d",i);
		CV_waitingInBackRoom[i]=new Condition(name);
    }
    //------------------ END OF CODE SEGMENT TO SET UP VALET------------------------------

    //----------------------------- CODE SEGMENT TO SET UP THE TICKET TAKERS ----------------
	
    //printf("\n The Total number of ticket takers in this simulation is %d",totalNumberOfTicketTakers);
   
    CV_noTicketTakers=new Condition("CV_noTicketTakers");
    totalNumberOfActiveTicketTakersLock=new Lock("totalNumberOfActiveTicketTakers");
    ticketTakerQueueLength=new int[totalNumberOfTicketTakers];
   
    ticketTakerQueueLengthLock=new Lock*[totalNumberOfTicketTakers];
   
    CV_waitingTicketTaker=new Condition*[totalNumberOfTicketTakers];

    CV_ticketTakerQueueLength=new Condition*[totalNumberOfTicketTakers];
    visitorMuseumEntryStatus=new int[totalNumberOfTicketTakers];
    visitorMuseumEntryStatusLock=new Lock*[totalNumberOfTicketTakers];
    CV_visitorMuseumEntryStatus=new Condition*[totalNumberOfTicketTakers];

    totalNumberOfVisitorsAcceptedLock=new Lock("totalNumberOfVisitorsAcceptedLock");
    if(totalNumberOfDrivers==0 && totalCountOfLimoDrivers==0)
    {
        totalNumberOfVisitorsAccepted=-1;
    }

    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
       
        ticketTakerQueueLength[i]=0; //SETTING THE INTIAL VALUE TO BE ZERO
        visitorMuseumEntryStatus[i]=0;
       
        name=new char[50];
        sprintf(name,"visitorMuseumEntryStatusLock%d",i);
        visitorMuseumEntryStatusLock[i]=new Lock(name);

        name=new char[50];
        sprintf(name,"CV_visitorMuseumEntryStatus%d",i);
        CV_visitorMuseumEntryStatus[i]=new Condition(name);

        name=new char[50];
        sprintf(name,"ticketTakerQueueLengthLock%d",i);
        ticketTakerQueueLengthLock[i]=new Lock(name);
       
        name=new char[50];
        sprintf(name,"CV_waitingTicketTaker%d",i);
        CV_waitingTicketTaker[i]=new Condition(name);
       
        name=new char[50];
        sprintf(name,"CV_ticketTakerQueueLength%d",i);
        CV_ticketTakerQueueLength[i]=new Condition(name);
       
    }
    //-------------------END OF CODE SEGMENT TO SET UP THE TICKET TAKERS -------------------------------
   


}


void TestCase4()
{

	char *name;
	printf("\n ********************** TEST CASE 3 STARTING UP **************************************");
	printf("\n **********************All the visitors wait since there is no ticket taker******************");
    
	setupTestCase4();

	 for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        printCarDetails(i);
    }
    for(int i=0;i<totalNumberOfValets;i++)
    {
        printValetDetails(i);
    }
    printf("\n");
    visitor=new int[grandTotalOfVisitors];
    for(int i=0;i<totalNumberOfCars+totalCountOfLimos;i++)
    {
        static int k=0;
        if(totalNumberOfVisitors[i]!=0)
        {
                for(int j=0;j<totalNumberOfVisitors[i];j++)
                {
                    visitor[k]=i;   
                    k++;
                }
        }
    }
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
		char *vehicle=new char[10];
		if(visitor[i]<totalNumberOfCars)
		{
			sprintf(vehicle,"car%d",visitor[i]);
			printf("\nvisitor %d is in  %s",i,vehicle);
		}
		else
		{
			sprintf(vehicle,"Limousine%d",visitor[i]);
			printf("\nvisitor %d is in %s",i,vehicle);
		}
       
    }
    // VALET THREADS ARE CREATED HERE
    //valetThread=new Thread[totalNumberOfValets];
    for(int i=0;i<totalNumberOfValets;i++)
    {
        name=new char[20];
        sprintf(name,"valetThread%d",i);
        valetThread=new Thread(name);
        valetThread->Fork((VoidFunctionPtr)valet,i);
    }

	// TICKET TAKER THREADS ARE CREATED
    for(int i=0;i<totalNumberOfTicketTakers;i++)
    {
        name=new char[20];
        sprintf(name,"ticketThread%d",i);
        ticketTakerThreadPtr=new Thread(name);
        ticketTakerThreadPtr->Fork((VoidFunctionPtr)ticketTakerFunction,i);
    }


	//LIMO DRIVER THREADS ARE CREATED HERE
    for(int i=totalNumberOfDrivers;i<totalCountOfLimoDrivers+totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"limoDriverThread%d",i);
		//printf("\n creating limo-driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)limoDriver,i);
    }

	//VISITOR THREADS ARE CREATED HERE
    for(int i=0;i<grandTotalOfVisitors;i++)
    {
        name=new char[20];
        sprintf(name,"visitorThread%d",i);
        visitorThreadPtr=new Thread(name);
        visitorThreadPtr->Fork((VoidFunctionPtr)visitorFunction,i);

    }
    //DRIVER THREADS ARE CREATED HERE
    for(int i=0;i<totalNumberOfDrivers;i++)
    {
        name=new char[20];
        sprintf(name,"driverThread%d",i);
		//printf("\n creating driver thread %d",i);
        driverThread=new Thread(name);
        driverThread->Fork((VoidFunctionPtr)driver,i);

    }
	valetManagerThreadPtr=new Thread("valetManager");
	valetManagerThreadPtr->Fork((VoidFunctionPtr)valetManager,-10);

	

    printf("\n");


}





