// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#include "table.h"
#include "synch.h"
#include <stdio.h>
#include <iostream.h>

extern "C" { int bzero(char *, int); };

Table::Table(int s) : map(s), table(0), lock(0), size(s) {
    table = new void *[size];
    lock = new Lock("TableLock");
}

Table::~Table() {
    if (table) {
	delete table;
	table = 0;
    }
    if (lock) {
	delete lock;
	lock = 0;
    }
}

void *Table::Get(int i) {
    // Return the element associated with the given if, or 0 if
    // there is none.

    return (i >=0 && i < size && map.Test(i)) ? table[i] : 0;
}

int Table::Put(void *f) {
    // Put the element in the table and return the slot it used.  Use a
    // lock so 2 files don't get the same space.
    int i;	// to find the next slot

    lock->Acquire();
    i = map.Find();
    lock->Release();
    if ( i != -1)
	table[i] = f;
    return i;
}

void *Table::Remove(int i) {
    // Remove the element associated with identifier i from the table,
    // and return it.

    void *f =0;

    if ( i >= 0 && i < size ) {
	lock->Acquire();
	if ( map.Test(i) ) {
	    map.Clear(i);
	    f = table[i];
	    table[i] = 0;
	}
	lock->Release();
    }
    return f;
}

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	"executable" is the file containing the object code to load into memory
//
//      It's possible to fail to fully construct the address space for
//      several reasons, including being unable to allocate memory,
//      and being unable to read key parts of the executable.
//      Incompletely consretucted address spaces have the member
//      constructed set to false.
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable) : fileTable(MaxOpenFiles)
{
    NoffHeader noffH;
    unsigned int i, size;

	unsigned int freePageNum,toPage,fromPage; // MCGS 

	unsigned int codePages,initDataPages;

    // Don't allocate the input or output to disk files
    fileTable.Put(0);
    fileTable.Put(0);

    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size;
    //numPages = divRoundUp(size, PageSize) + divRoundUp(UserStackSize,PageSize);
	numPages = divRoundUp(size, PageSize) + divRoundUp((UserStackSize*MaxThreadCountPerProcess),PageSize);

	codePages=divRoundDown(noffH.code.size,PageSize);
	initDataPages=divRoundUp(noffH.initData.size,PageSize);


    size = numPages * PageSize;
	stackPage=0;
	stackPageLock=new Lock("stackPageLock");
    //ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
	// first, set up the translation 
    pageTable = new PageTable[numPages];
	fromPage=noffH.code.inFileAddr; // SETTING UP INTIAL POSITION TO READ FROM IN THE EXECUTABLE

	processIdLock->Acquire(); // ACQUIRING THE LOCK TO MAKE SURE THAT THE FIND OPERATION IS MUTUALLY EXCLUSINVE
	processNumber=findProcessId->Find(); // FINDING AN UN-USED PROCESS NUMBER TO ASSOCIATE WITH THIS PROCESS
	if(processNumber==-1)
	{
		printf("\n !!!! Nachos : Sorry max process count reached !!!! \n");
		interrupt->Halt();
	}
	processIdLock->Release(); // FIND IS DONE SO REALESING THE LOCK 
    for (i = 0; i < numPages; i++) 
	{
			pageTable[i].virtualPage = i;	// for now, virtual page # = phys page # // MCGS VIRT PAGE DOES NOT CHANGE
			pageTable[i].physicalPage=-1;
			pageTable[i].readOnly=FALSE;
			pageTable[i].valid=TRUE;
			pageTable[i].dirty=FALSE;
			pageTable[i].pageLocation=EXE;
			pageTable[i].diskLocation = fromPage;
			fromPage=fromPage+PageSize; // MCGS 2:  INCREMENTING THE SOURCE POINTER AFTER READING IT	
	}

	proTableLock->Acquire();  // ACQUIRING THE PROCESS TABLE LOCK
	proTable[processNumber].processID=processNumber; // UPDATING THE THE PROCESS NUMBER INTO THE PROCESS TABLE
	proTable[processNumber].threadCount++; // INCREMENTING THE THREAD COUNT
	proTable[processNumber].exeHandle=executable; // SAVING THE EXE HANDLE
	proTableLock->Release(); // RELEASING THE LOCK 

	runningProcessCountLock->Acquire();
	runningProcessCount++;
	runningProcessCountLock->Release();




    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
// bzero(machine->mainMemory, size); // MCGS : COMMENTED BY ME 


} // END OF ADDRESS SPACE CONSTRUCTOR

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
//
// 	Dealloate an address space.  release pages, page tables, files
// 	and file tables
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
    delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %x\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    //machine->pageTable = pageTable; // MCGS : COMMENTED AS PER THE INSTRUCTIONS IN STEP 1 IN PROJECT 3
	//machine->pageTableSize = numPages; // MCGS : COMMENTED AS PER THE INSTRUCTIONS IN STEP 1 IN PROJECT 3
	//-------------------------------- MCGS 3: INVALIDATING THE TLB --------------------------------------------//
	IntStatus oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERRUPT
	//printf("\n !!!! Nachos : Invalidating TLB. !!!!");
	for(int i=0;i<TLBSize;i++)
	{
		if((machine->tlb[i].dirty==TRUE) && (machine->tlb[i].valid==TRUE))
		{
			//cout<<"\n================================> Inside Restore AddressSpace and the dirty bit is true"<<endl;
			int physPage=machine->tlb[i].physicalPage;
			int virtPage=machine->tlb[i].virtualPage;
			ipt[physPage].dirty=machine->tlb[i].dirty;
			ipt[physPage].pSpace->pageTable[virtPage].dirty=machine->tlb[i].dirty;
		}
		machine->tlb[i].valid=FALSE;
	}
	(void) interrupt->SetLevel(oldLevel); // RESTORING THE INTERRUPT
	//------------------------------- END OF MCGS 3 ----------------------------------------------------------//
}
