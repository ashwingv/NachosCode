// system.h 
//	All global variables used in Nachos are defined here.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SYSTEM_H
#define SYSTEM_H

#include "copyright.h"
#include "utility.h"
#include "thread.h"
#include "scheduler.h"
#include "interrupt.h"
#include "stats.h"
#include "timer.h"

#define MaxLocksConditions 10000	// MCGS : TO KEEP TRACK OF THE TOTAL NUMBER OF LOCKS AVAILABLE FOR THAT PROCESS

#define MaxProcessCount 10	// MCGS : TO KEEP TRACK OF THE MAX COUNT OF THE PROCESS IN THE SYSTYEM

#define MaxThreadCountPerProcess 50	// MCGS : TO KEEP TRACK OF THE MAX ALLOWED THREADS PER PROCESS

#define MaxRpcLocksCondtions 1000


class BitMap;
class AddrSpace;
class Lock;
class Condition;
class OpenFile;
//struct LockProRelation;
//struct ConditionProRelation;

struct processTable								// MCGS : PROCESS TABLE STRUCTURE 
{
	int processID;			// A INTEGER TO KEEP TRACK OF THE PROCESS ID							 
	int threadCount;		// A COUNTER TO KEEP TRACK OF THE THREADS IN THE PROCESS
	OpenFile *exeHandle;
};

// ---------------------------  MCGS 3: FOR NETWORK ---------------------------------------------


struct remoteLock
{
  char name[50]; 
  int owner;     
  List *waitQueue; 
 
};

struct remoteCondition
{
  char name[50];   
  int lockId;       
  List *waitQueue;   
 
};





extern  remoteLock *rpcLocks;
extern  remoteCondition *rpcConditions;

extern BitMap *rpcLocksBM;
extern Lock *rpcLocksBMLock;
extern BitMap *rpcConditionsBM;
extern Lock *rpcConditionsBMLock;



// --------------------------------- END OF MCGS 3: NETWORK ----------------------------------------





// Initialization and cleanup routines
extern void Initialize(int argc, char **argv); 	// Initialization,
						// called before anything else
extern void Cleanup();				// Cleanup, called when
						// Nachos is done.

extern Thread *currentThread;			// the thread holding the CPU
extern Thread *threadToBeDestroyed;  		// the thread that just finished
extern Scheduler *scheduler;			// the ready list
extern Interrupt *interrupt;			// interrupt status
extern Statistics *stats;			// performance metrics
extern Timer *timer;				// the hardware alarm clock


//------------------- MCGS  ------------------------------//

extern BitMap *findLockBitMap;						// MCGS 2: A BIT MAP TO FIND WHICH LOCK IS FREE
extern BitMap *findConditionBitMap;					// MCGS 2: A BIT MAP TO FIND WHICH CONDITON VARIABLE IS FREE

extern Lock *findLockBitMapMutex;					// MCGS 2: LOCK TO CONTROL ACCESS TO FIND A FREE NUM FOR A LOCK 
extern Lock *findConditionBitMapMutex;				// MCGS 2: LOCK TO CONTROL ACCESS TO FIND A FREE NUM FOR A CONDITION VARIABLE 

extern Lock *lockArr[MaxLocksConditions];			// MCGS 2: GLOBAL ARRAY OF LOCKS POINTERS AVAILABLE IN THE SYSTEM
extern AddrSpace *lockAddrSpace[MaxLocksConditions];

extern Condition *conditionArr[MaxLocksConditions]; // MCGS 2: GLOBAL ARRAY OF CONDITION VARIABLES POINTERS AVAILABLE IN THE SYSTEM
extern AddrSpace *conditionAddrSpace[MaxLocksConditions];


extern BitMap *findPhysicalPage;					// MCGS 2: A BIT MAP TO FIND WHICH PHYSICAL PAGE IN MEMORY IS FREE
extern Lock *physicalPageLock;						// MCGS 2: A LOCK TO CONTROL ACCESS TO THE BIT MAP AND PAGE ALLOCATION

extern processTable *proTable;						// MCGS 2: A PROCES TABLE POINTER
extern Lock *proTableLock;							// MCGS 2: A LOCK TO CONTROL ACCESS TO THE PROCESS TABLE

extern BitMap *findProcessId;						// MCGS 2: A BITMAP VARIABLE TO FIND A UNSUED PROCESS NUMBER
extern Lock *processIdLock;							// MCGS 2: A LOCK TO CONTROL ACCESS TO THE BIT MAP THAT FINDS A UNUSED PROCESS ID

//extern Thread *execThread;						// MCGS 2:  A THREAD POINTER USED TO CREATE THE MAIN THREAD IN THE EXEC-ED PROCESS
//extern Thread *forkedThread;						// MCGS 2:  A THREAD POINTER USED TO CREATE CREATE A NEW CHILD THREAD INSIDE A PROCESS

extern int runningProcessCount;
extern Lock *runningProcessCountLock;

//----------------------- MCGS 3----------------------------------------------------//

extern int currentTLB;				// MCGS 3:  A COUNTER TO KEEP TRACK OF THE INDEX VALUE OF TLB
enum PageLocation {EXE,RAM,SWAP,NOWHERE};
enum Replace {FIFO,RAND};

extern int replacePolicy;

extern int fifoPage;


class Ipt
{
	public:
    int virtualPage;  				// The page number in virtual memory.
    int physicalPage;  				// The page number in real memory (relative to the
									//  start of "mainMemory"
    bool valid;						// If this bit is set, the translation is ignored.
									// (In other words, the entry hasn't been initialized.)
    bool readOnly;					// If this bit is set, the user program is not allowed
									// to modify the contents of the page.
    bool use;						// This bit is set by the hardware every time the
									// page is referenced or modified.
    bool dirty;						// This bit is set by the hardware every time the
									// page is modified.

	int pid;						//  AN INTEGER TO HOLD THE PROCESS ID	
	AddrSpace *pSpace;
};

/* ------- MCGS 3: ADDED FOR STEP 3 A USER DEFINED PAGE TABLE --------*/
class PageTable
{
	public:
    int virtualPage;  				// The page number in virtual memory.
	int physicalPage;				// 
	bool readOnly;					// TO INDICATE WHETHER THE PAGE IS READ ONLY OR NOT
	bool valid;						// TO INDICATE THE PAGE IS IN THE PHYSICAL MEMORY OR NOT
	bool use;						// 
	bool dirty;						// TO INDICATE WHETHER THE PAGE IS DIRTY OR NOT
	int pageLocation;				// 
    int diskLocation;  				// The page number in real memory (relative to the
									//  start of "mainMemory"
};

extern Ipt *ipt;					//MCGS 3: EXTERN DECLARATION OF IPT
extern Lock *iptLock;				//MCGS 3: EXTERN DECLARATION OF IPT LOCK 
extern BitMap *swapfileBitMap;		//MCGS 3: EXTERN DECLARATION OF SWAPFILE BITMAP
extern OpenFile *swapfile;			//MCGS 3: EXTERN DECLATAION OF THE SWAPFILE HANDLE

extern Lock *swapfileBitMapLock;

extern Lock *swapFileLock;


//extern int iptIndex;			//MCGS 3: A COUNTER TO THE INDEX POSITION





//------------------ END OF MCGS -------------------------//

#ifdef USER_PROGRAM
#include "machine.h"
extern Machine* machine;	// user program memory and registers
#endif

#ifdef FILESYS_NEEDED 		// FILESYS or FILESYS_STUB 
#include "filesys.h"
extern FileSystem  *fileSystem;
#endif

#ifdef FILESYS
#include "synchdisk.h"
extern SynchDisk   *synchDisk;
#endif

#ifdef NETWORK
#include "post.h"
extern PostOffice* postOffice;
#endif

#endif // SYSTEM_H
