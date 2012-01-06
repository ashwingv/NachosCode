// system.cc 
//	Nachos initialization and cleanup routines.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "synch.h" // MCGS


// This defines *all* of the global data structures used by Nachos.
// These are all initialized and de-allocated by this file.

Thread *currentThread;			// the thread we are running now
Thread *threadToBeDestroyed;  		// the thread that just finished
Scheduler *scheduler;			// the ready list
Interrupt *interrupt;			// interrupt status
Statistics *stats;			// performance metrics
Timer *timer;				// the hardware timer device,
					// for invoking context switches


//------------------------ MCGS ---------------- //

//Thread *execThread;							// MCGS :  A THREAD POINTER USED TO CREATE THE MAIN THREAD IN THE EXEC-ED PROCESS
//Thread *forkedThread;							// MCGS :  A THREAD POINTER USED TO CREATE CREATE A NEW CHILD THREAD INSIDE A PROCESS

BitMap *findLockBitMap;							// MCGS : A BIT MAP TO FIND WHICH LOCK IS FREE
BitMap *findConditionBitMap;					// MCGS : A BIT MAP TO FIND WHICH CONDITON VARIABLE IS FREE

Lock *findLockBitMapMutex;						// MCGS : A BIT MAP TO FIND WHICH PHYSICAL PAGE IN MEMORY IS FREE
Lock *findConditionBitMapMutex;					// MCGS : A LOCK TO CONTROL ACCESS TO THE BIT MAP AND PAGE ALLOCATION

Lock *lockArr[MaxLocksConditions];				// MCGS : ARRAY OF LOCKS AVAILABLE IN THE SYSTEM.
AddrSpace *lockAddrSpace[MaxLocksConditions];

Condition *conditionArr[MaxLocksConditions];	// MCGS : ARRAY OF CONDITIONS VARIABLES AVAILABLE IN THE SYSTEM
AddrSpace *conditionAddrSpace[MaxLocksConditions];

BitMap *findPhysicalPage;						// MCGS : A BIT MAP TO FIND AN UNUSED PHYSICAL PAGE
Lock *physicalPageLock;							// MCGS : LOCK TO CONTROL ACCESS TO THE BITMAP AND PAGE ALLOCATION 

processTable *proTable;							// MCGS : A PROCES TABLE POINTER
Lock *proTableLock;								// MCGS : A LOCK TO CONTROL ACCESS TO THE PROCESS TABLE

BitMap *findProcessId;							// MCGS : A BITMAP VARIABLE TO FIND A UNSUED PROCESS NUMBER
Lock *processIdLock;							// MCGS : A LOCK TO CONTROL ACCESS TO THE BIT MAP THAT FINDS A UNUSED PROCESS ID


int runningProcessCount=0;						// MCGS :
Lock *runningProcessCountLock;					// MCGS :

//----------------- END OF MCGS ---------------------//

//----------------------------------- MCGS : FOR PORJECT 3 ----------------------------------------//

int currentTLB=0;								// MCGS 3:  A COUNTER TO KEEP TRACK OF THE INDEX VALUE OF TLB

Ipt *ipt;
Lock *iptLock;


BitMap *swapfileBitMap; // MCGS 3: A BIT MAP TO FIND A FREE LOCATION IN A SWAP FILE.
Lock *swapfileBitMapLock;

OpenFile *swapfile;		// MCGS 3: A OPEN FILE POINTER TO ACCESS THE SWAP FILE.
Lock *swapFileLock;

int replacePolicy=FIFO;

int fifoPage=-1;


//--------------------- MCGS 3 : FOR NETWORKING --------------------------

remoteLock *rpcLocks;
remoteCondition *rpcConditions;

BitMap *rpcLocksBM;
Lock *rpcLocksBMLock;
BitMap *rpcConditionsBM;
Lock *rpcConditionsBMLock;

//-------------------------------------------------------------------------


//-------------------------------------- END OF MCGS ----------------------------------------------//

#ifdef FILESYS_NEEDED
FileSystem  *fileSystem;
#endif

#ifdef FILESYS
SynchDisk   *synchDisk;
#endif

#ifdef USER_PROGRAM	// requires either FILESYS or FILESYS_STUB
Machine *machine;	// user program memory and registers
#endif

#ifdef NETWORK
PostOffice *postOffice;
#endif


// External definition, to allow us to take a pointer to this function
extern void Cleanup();


//----------------------------------------------------------------------
// TimerInterruptHandler
// 	Interrupt handler for the timer device.  The timer device is
//	set up to interrupt the CPU periodically (once every TimerTicks).
//	This routine is called each time there is a timer interrupt,
//	with interrupts disabled.
//
//	Note that instead of calling Yield() directly (which would
//	suspend the interrupt handler, not the interrupted thread
//	which is what we wanted to context switch), we set a flag
//	so that once the interrupt handler is done, it will appear as 
//	if the interrupted thread called Yield at the point it is 
//	was interrupted.
//
//	"dummy" is because every interrupt handler takes one argument,
//		whether it needs it or not.
//----------------------------------------------------------------------
static void
TimerInterruptHandler(int dummy)
{
    if (interrupt->getStatus() != IdleMode)
	interrupt->YieldOnReturn();
}

//----------------------------------------------------------------------
// Initialize
// 	Initialize Nachos global data structures.  Interpret command
//	line arguments in order to determine flags for the initialization.  
// 
//	"argc" is the number of command line arguments (including the name
//		of the command) -- ex: "nachos -d +" -> argc = 3 
//	"argv" is an array of strings, one for each command line argument
//		ex: "nachos -d +" -> argv = {"nachos", "-d", "+"}
//----------------------------------------------------------------------
void
Initialize(int argc, char **argv)
{
    int argCount;
    char* debugArgs = "";
    bool randomYield = FALSE;
	//pageTable = new TranslationEntry[numPages]; MCGS -------REF

	swapfileBitMapLock=new Lock("swapfileBitMapLock");

	swapFileLock=new Lock("swapFileLock");




#ifdef USER_PROGRAM
    bool debugUserProg = FALSE;	// single step user program

	findLockBitMap=new BitMap(MaxLocksConditions); // MCGS
	findConditionBitMap=new BitMap(MaxLocksConditions); // MCGS

	findLockBitMapMutex=new Lock("findLockBitMapMutex"); // MCGS
	findConditionBitMapMutex=new Lock("findConditionBitMapMutex"); // MCGS

	findPhysicalPage=new BitMap(NumPhysPages);	// MCGS : A BIT MAP POINTER OF SIZE EQUAL TO THE SIZE OF THE PHYSICAL MEMORY
												// MCGS : USED TO FIND A FREE PAGE IN MEMORY
	physicalPageLock=new Lock("physicalPageLock"); // MCGS : LOCK THAT IS USED TO CONTROL ACCESS TO THE BIT MAP CREATED ABOVE

	proTable=new processTable[MaxProcessCount];	// MCSG : A PROCESS TABLE TO CAPTURE THE INFORMATION ABOUT THE PROCESS
	proTableLock=new Lock("proTableLock"); // MCGS : A LOCK TO CONTROL ACCESS TO THE PROCESS TABLE

	findProcessId=new BitMap(MaxProcessCount);	// MCGS : A BIT MAP POINTER TO FIND A UNUSED PROCESS NUMBER	
	processIdLock=new Lock("processIdLock");	//MCGS : A LOCK TO CONTROL ACCESS TO THE BIT MAP THAT FINDS A UNSUED PROCESS ID

	runningProcessCountLock=new Lock("runningProcessCountLock");

	swapfileBitMap=new BitMap(65536);	// MCGS 3: A BIT MAP OF SIZE 65536 FOR THE SWAP FILE IS CREATED.
	ipt=new Ipt[NumPhysPages];			// MCGS 3: IPT IS CREATED.
	iptLock=new Lock("iptLock");		// MCGS 3: A LOCK FOR IPT IS CREATED.

	for(int i=0;i<NumPhysPages;i++)
	{
		ipt[i].virtualPage=-1;
		ipt[i].physicalPage=-1;
		ipt[i].valid=FALSE;
		ipt[i].dirty=FALSE;
	}

	//------------ MCGS 3: PART 3 NETWORKS -------------

rpcLocks=new remoteLock[MaxRpcLocksCondtions];
rpcConditions=new remoteCondition[MaxRpcLocksCondtions];

for(int i=0;i<MaxRpcLocksCondtions;i++)
{
	
	rpcLocks[i].owner=-1;
	rpcLocks[i].waitQueue=new List();

    rpcConditions[i].lockId=-1;
	rpcConditions[i].waitQueue=new List();
}
rpcLocksBM= new BitMap(MaxRpcLocksCondtions);
rpcLocksBMLock=new Lock("rpcLocksBMLock");
rpcConditionsBM=new BitMap(MaxRpcLocksCondtions);
rpcConditionsBMLock=new Lock("rpcConditionsBMLock");

	

#endif

#ifdef FILESYS_NEEDED
    bool format = FALSE;	// format disk
#endif

#ifdef NETWORK
    double rely = 1;		// network reliability
    int netname = 0;		// UNIX socket name
#endif
    
    for (argc--, argv++; argc > 0; argc -= argCount, argv += argCount) {
	argCount = 1;
	if (!strcmp(*argv, "-d")) {
	    if (argc == 1)
		debugArgs = "+";	// turn on all debug flags
	    else {
	    	debugArgs = *(argv + 1);
	    	argCount = 2;
	    }
	} else if (!strcmp(*argv, "-rs")) {
	    ASSERT(argc > 1);
	    RandomInit(atoi(*(argv + 1)));	// initialize pseudo-random
						// number generator
	    randomYield = TRUE;
	    argCount = 2;
	}
#ifdef USER_PROGRAM
	if (!strcmp(*argv, "-s"))
	    debugUserProg = TRUE;
#endif
#ifdef FILESYS_NEEDED
	if (!strcmp(*argv, "-f"))
	    format = TRUE;
#endif
#ifdef NETWORK
	if (!strcmp(*argv, "-l")) {
	    ASSERT(argc > 1);
	    rely = atof(*(argv + 1));
	    argCount = 2;
	} else if (!strcmp(*argv, "-m")) {
	    ASSERT(argc > 1);
	    netname = atoi(*(argv + 1));
	    argCount = 2;
	}
#endif
    }

    DebugInit(debugArgs);			// initialize DEBUG messages
    stats = new Statistics();			// collect statistics
    interrupt = new Interrupt;			// start up interrupt handling
    scheduler = new Scheduler();		// initialize the ready queue
    if (randomYield)				// start the timer (if needed)
	timer = new Timer(TimerInterruptHandler, 0, randomYield);

    threadToBeDestroyed = NULL;

    // We didn't explicitly allocate the current thread we are running in.
    // But if it ever tries to give up the CPU, we better have a Thread
    // object to save its state. 
    currentThread = new Thread("main");		
    currentThread->setStatus(RUNNING);

    interrupt->Enable();
    CallOnUserAbort(Cleanup);			// if user hits ctl-C
    
#ifdef USER_PROGRAM
    machine = new Machine(debugUserProg);	// this must come first
#endif

#ifdef FILESYS
    synchDisk = new SynchDisk("DISK");
#endif

#ifdef FILESYS_NEEDED
    fileSystem = new FileSystem(format);
	if(!(fileSystem->Create("swapfile",65536)))
	{
	  printf("ERROR : Creating Swap File");
	  ASSERT(FALSE);
	}
	   swapfile = fileSystem->Open("swapfile");
#endif

#ifdef NETWORK
    postOffice = new PostOffice(netname, rely, 10);
#endif
}

//----------------------------------------------------------------------
// Cleanup
// 	Nachos is halting.  De-allocate global data structures.
//----------------------------------------------------------------------
void
Cleanup()
{
    printf("\nCleaning up...\n");
#ifdef NETWORK
    delete postOffice;
#endif
    
#ifdef USER_PROGRAM
    delete machine;
#endif

#ifdef FILESYS_NEEDED
    delete fileSystem;
#endif

#ifdef FILESYS
    delete synchDisk;
#endif
    
    delete timer;
    delete scheduler;
    delete interrupt;
    
    Exit(0);
}

