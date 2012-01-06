// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//Close
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "synch.h" // MCGS
#include <stdio.h>
#include <iostream>
#include "addrspace.h" // MCGS
//#include "machine.h";

using namespace std;

int copyin(unsigned int vaddr, int len, char *buf) {
    // Copy len bytes from the current thread's virtual address vaddr.
    // Return the number of bytes so read, or -1 if an error occors.
    // Errors can generally mean a bad virtual address was passed in.
    bool result;
    int n=0;			// The number of bytes copied in
    int *paddr = new int;

    while ( n >= 0 && n < len) 
	{
      result = machine->ReadMem( vaddr, 1, paddr );
	  while(!result) // MCGS 3: TO HANDLE PAGE FAULT IN THE ReadMem SYS CALL
		{
		  result = machine->ReadMem( vaddr, 1, paddr ); // MCGS 3: TO HANDLE PAGE FAULT IN THE ReadMem SYS CALL
		}

      buf[n++] = *paddr;

      if ( !result ) {
	//translation failed
	return -1;
      }

      vaddr++;
    }

    delete paddr;
    return len;
}

int copyout(unsigned int vaddr, int len, char *buf) {
    // Copy len bytes to the current thread's virtual address vaddr.
    // Return the number of bytes so written, or -1 if an error
    // occors.  Errors can generally mean a bad virtual address was
    // passed in.
    bool result;
    int n=0;			// The number of bytes copied in

    while ( n >= 0 && n < len) {
      // Note that we check every byte's address
      result = machine->WriteMem( vaddr, 1, (int)(buf[n++]) );

      if ( !result ) {
	//translation failed
	return -1;
      }

      vaddr++;
    }

    return n;
}

void Create_Syscall(unsigned int vaddr, int len) {
    // Create the file with the name in the user buffer pointed to by
    // vaddr.  The file name is at most MAXFILENAME chars long.  No
    // way to return errors, though...
    char *buf = new char[len+1];	// Kernel buffer to put the name in

    if (!buf) return;

    if( copyin(vaddr,len,buf) == -1 ) {
	printf("%s","Bad pointer passed to Create\n");
	delete buf;
	return;
    }

    buf[len]='\0';

    fileSystem->Create(buf,0);
    delete[] buf;
    return;
}

int Open_Syscall(unsigned int vaddr, int len) {
    // Open the file with the name in the user buffer pointed to by
    // vaddr.  The file name is at most MAXFILENAME chars long.  If
    // the file is opened successfully, it is put in the address
    // space's file table and an id returned that can find the file
    // later.  If there are any errors, -1 is returned.
    char *buf = new char[len+1];	// Kernel buffer to put the name in
    OpenFile *f;			// The new open file
    int id;				// The openfile id

    if (!buf) {
	printf("%s","Can't allocate kernel buffer in Open\n");
	return -1;
    }

    if( copyin(vaddr,len,buf) == -1 ) {
	printf("%s","Bad pointer passed to Open\n");
	delete[] buf;
	return -1;
    }

    buf[len]='\0';

    f = fileSystem->Open(buf);
    delete[] buf;

    if ( f ) {
	if ((id = currentThread->space->fileTable.Put(f)) == -1 )
	    delete f;
	return id;
    }
    else
	return -1;
}

void Write_Syscall(unsigned int vaddr, int len, int id) {
    // Write the buffer to the given disk file.  If ConsoleOutput is
    // the fileID, data goes to the synchronized console instead.  If
    // a Write arrives for the synchronized Console, and no such
    // console exists, create one. For disk files, the file is looked
    // up in the current address space's open file table and used as
    // the target of the write.
    
    char *buf;		// Kernel buffer for output
    OpenFile *f;	// Open file for output

    if ( id == ConsoleInput) return;
    
    if ( !(buf = new char[len]) ) {
	printf("%s","Error allocating kernel buffer for write!\n");
	return;
    } else {
        if ( copyin(vaddr,len,buf) == -1 ) {
	    printf("%s","Bad pointer passed to to write: data not written\n");
	    delete[] buf;
	    return;
	}
    }

    if ( id == ConsoleOutput) {
      for (int ii=0; ii<len; ii++) {
	printf("%c",buf[ii]);
      }

    } else {
	if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
	    f->Write(buf, len);
	} else {
	    printf("%s","Bad OpenFileId passed to Write\n");
	    len = -1;
	}
    }

    delete[] buf;
}

int Read_Syscall(unsigned int vaddr, int len, int id) {
    // Write the buffer to the given disk file.  If ConsoleOutput is
    // the fileID, data goes to the synchronized console instead.  If
    // a Write arrives for the synchronized Console, and no such
    // console exists, create one.    We reuse len as the number of bytes
    // read, which is an unnessecary savings of space.
    char *buf;		// Kernel buffer for input
    OpenFile *f;	// Open file for output

    if ( id == ConsoleOutput) return -1;
    
    if ( !(buf = new char[len]) ) {
	printf("%s","Error allocating kernel buffer in Read\n");
	return -1;
    }

    if ( id == ConsoleInput) {
      //Reading from the keyboard
      scanf("%s", buf);

      if ( copyout(vaddr, len, buf) == -1 ) {
	printf("%s","Bad pointer passed to Read: data not copied\n");
      }
    } else {
	if ( (f = (OpenFile *) currentThread->space->fileTable.Get(id)) ) {
	    len = f->Read(buf, len);
	    if ( len > 0 ) {
	        //Read something from the file. Put into user's address space
  	        if ( copyout(vaddr, len, buf) == -1 ) {
		    printf("%s","Bad pointer passed to Read: data not copied\n");
		}
	    }
	} else {
	    printf("%s","Bad OpenFileId passed to Read\n");
	    len = -1;
	}
    }

    delete[] buf;
    return len;
}

void Close_Syscall(int fd) 
{
    // Close the file associated with id fd.  No error reporting.
    OpenFile *f = (OpenFile *) currentThread->space->fileTable.Remove(fd);

    if ( f ) {
      delete f;
    } else {
      printf("%s","Tried to close an unopen file\n");
    }
}

// -----------------------------------  MCGS ---------------------------------------------------------- //

/*-----------------------------------------------------------------------------
YIELD SYSTEM CALL KERNEL CODE. WHEN CALLED THE CURRENT THREAD YIELDS THE CPU.
-----------------------------------------------------------------------------*/
void Yield_Syscall()
{
	//printf("\n calling yield \n");
	currentThread->Yield();
}

/*-----------------------------------------------------------------------------
CREATE LOCK SYSTEM CALL KERNEL CODE. WHEN CALLED RETURNS AN UNIQUE INTEGER
WHICH IS THE LOCK REFERENCE TO THE USER PROGRAM
-----------------------------------------------------------------------------*/
int CreateLock_Syscall(int dummyName,int nameLen)
{

	// --------------------- MCGS :3 FOR RPCS ------------------------------------------------
	#ifdef NETWORK
		PacketHeader outPktHdr, inPktHdr;
	    MailHeader outMailHdr, inMailHdr;
		char *lockName=new char[32];
		char msgData[32]="101";
		if( copyin(dummyName,nameLen,lockName) == -1 ) 
		{
			cout<<"\n !!!!! Nachos Bad pointer passed to function !!!!"<<endl;
			return -1;
		}
		lockName[nameLen]='\0';
		strcat(msgData,lockName);
		
		// CONSTRUCTING THE MESSAGE HEADER
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgData) + 1;
		
		// TRYING TO SEND MESSAGE TO SERVER
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgData);  //Sending the data to the server to perform the task

		if ( !success ) // TRYING IF SEND IS SUCCESSFULL
		{
			cout<<"\n !!!! Nachos : Create Lock send request failed  !!!!"<<endl;			  
			interrupt->Halt();
		}
		postOffice->Receive(1, &inPktHdr, &inMailHdr,msgData);
		cout<<"\n Nachos !!!! : The Lock reference received from the server is "<<msgData<<endl;
		cout<<"\n Nachos !!!! : The created lock is "<<msgData<<endl;
		int lockRef;
		lockRef=atoi(msgData);
		return lockRef;
	#endif
	// --------------------- MCGS :3 FOR RPCS ------------------------------------------------

	int ID;
	char *name=new char[20]; // CHARACTER ARRAY TO HOLD THE LOCK NAME
	findLockBitMapMutex->Acquire();
	ID=findLockBitMap->Find();
	if(ID==-1) // CHECKING IF ANY FREE LOCK REFERENCE IS AVAILABLE
	{
		// NO MAX LOCK REACHED
		printf("\n !!!! Nachos : You cannot create any more locks. Max lock count reached. !!!! ");
		findLockBitMapMutex->Release();
		return -1;
	}
	else
	{
		// STILL LOCK REFERENCE ARE AVAILABLE
		sprintf(name,"Lock#%d",ID); 
		lockArr[ID]=new Lock(name); // CREATING A LOCK 
		lockAddrSpace[ID]=currentThread->space; // ASSOCIATING THE LOCK WITH A ADDRESS SPACE
		printf("\n !!!!! Nachos : A lock is created with ID : %d and is within the address space %u . !!!!\n",ID,currentThread->space);
	}
	findLockBitMapMutex->Release();
	return ID; // RETURNING THE LOCK ID

}

/*-----------------------------------------------------------------------------
DESTROY LOCK SYSTEM CALL KERNEL CODE. WHEN CALLED DESTROYS THE LOCK.
TAKES A INTEGER AS THE LOCK REFERENCE FROM THE USER PROGRAM.
-----------------------------------------------------------------------------*/
int DestroyLock_Syscall(int ref)
{

	//---------------------------------------- MCGS 3: FOR RPC ----------------------------------------------
		#ifdef NETWORK
		PacketHeader outPktHdr, inPktHdr;
	    MailHeader outMailHdr, inMailHdr;
		char *lockNum=new char[32];
		char msgData[32]="104 ";
		sprintf(lockNum,"%d",ref);
		strcat(msgData,lockNum);
		
		
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgData) + 1;
		
		// TRYING TO SEND MESSAGE TO SERVER
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgData);  //Sending the data to the server to perform the task

		if ( !success ) // TRYING IF SEND IS SUCCESSFULL
		{
			cout<<"\n !!!! Nachos : Destroy Lock send request failed  !!!!"<<endl;			  
			interrupt->Halt();
		}
		char recvdMsg[32];
		postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
		//cout<<"\n Nachos !!!! : The Lock reference received from the server is "<<recvdMsg<<endl;
		//cout<<"\n Nachos !!!! : The destroyed lock is "<<recvdMsg<<endl;
		int lockRef;
		lockRef=atoi(recvdMsg);
		if(lockRef<0)
		{
			cout<<"\n !!!! Nachos : Either is lock is not yet created or its busy cannot destroy lock now !!!!"<<endl;
			return -1;
		}
		else
		{
			cout<<"\n !!!! Nachos  : The Lock reference received from the server is "<<recvdMsg<<endl;
			cout<<"\n !!!! Nachos  : The destroyed lock is "<<recvdMsg<<endl;
			return lockRef;
		}
		
		
		#endif
	//---------------------------------------- END  MCGS 3: FOR RPC ----------------------------------------------

	bool state;
	int ID=machine->ReadRegister(4); // GETTING THE REFERENCE OF THE LOCK THAT HAS TO BE DESTROYED
	if(ID<0 || ID >MaxLocksConditions) // CHECKING FOR THE VALID ID
	{
		// INVALID ID NOT WITHIN THE RANGE
		printf("\n !!!! Nachos: Invalid Lock Reference. !!!! \n");
		return -1;
	}
	// YES WITHIN THE RANGE
	findLockBitMapMutex->Acquire();
	state=findLockBitMap->Test(ID); 
	if(state)// CHECKING IF THE LOCK IS CREATED OR NOT
	{
			// YES THE LOCK IS CREATED
			if(lockAddrSpace[ID]==currentThread->space) // CHEKCING TO SEE IF THE CURRENT THREAD HAVE PERMISSION TO DELETE THIS LOCK 
			{
				if(lockArr[ID]->status!=1) // CHECKING TO SEE IF THE STATUS OF THE LOCK IS BUSY 
				{
					findLockBitMap->Clear(ID);
					delete(lockArr[ID]); // DELETING THE LOCK 
					lockAddrSpace[ID]=NULL; // CLEARING THE ADDRESS SPACE
					printf("\n !!!! Nachos : The destroyed lock is %d . !!!! \n",ID);
					findLockBitMapMutex->Release();
					return 0;
				}
				else
				{
					// THE LOCK IS BUSY YOU CANNOT DESTROY 
					printf("\n !!!! Nachos : Sorry you cannot the destroy a lock which is busy or one or more thread(s) are waiting on this lockqueue. !!!!\n");
					findLockBitMapMutex->Release();
					return -1;

				}
			}
			else
			{
				findLockBitMapMutex->Release();
				// THE CURRENT THREAD IS FROM A DIFFERENT ADDRESS SPACE
				printf("\n !!!!!Nachos : The thread does not have permission to access this lock. !!!! \n");
				return -1;

			}
	
	}
	else
	{
		// THE LOCK IS NOT YET CREATED 
		printf("\n !!!! Nachos : The Lock with number %d is not yet created. !!!! \n",ID);
		findLockBitMapMutex->Release();

	}
}

/*-----------------------------------------------------------------------------
ACCQUIRE LOCK SYSTEM CALL KERNEL CODE  WHEN CALLED  ACQUIRES THE LOCK 
FOR THE USER PROGRAM. TAKES A INTEGER AS THE LOCK REFERENCE.
-----------------------------------------------------------------------------*/
int AcquireLock_Syscall(int ref)
{

	#ifdef NETWORK

		PacketHeader outPktHdr, inPktHdr;
	    MailHeader outMailHdr, inMailHdr;
		char *lockNum=new char[32];
		char msgData[32]="102 ";
		sprintf(lockNum,"%d",ref);
		strcat(msgData,lockNum);
		
		// CONSTRUCTING THE MESSAGE HEADER
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgData) + 1;
		
		// TRYING TO SEND MESSAGE TO SERVER
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgData);  //Sending the data to the server to perform the task

		if ( !success ) // TRYING IF SEND IS SUCCESSFULL
		{
			cout<<"\n !!!! Nachos : Acquire Lock send request failed  !!!!"<<endl;			  
			interrupt->Halt();
		}
		char recvdMsg[32];
		postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
		//cout<<"\n Nachos !!!! : The Lock reference received from the server is "<<recvdMsg<<endl;
		//cout<<"\n Nachos !!!! : The Acquire Lock is "<<recvdMsg<<endl;
		int lockRef;
		lockRef=atoi(recvdMsg);
		if(lockRef<0)
		{
			cout<<"\n !!!! Nachos : The Lock that the process tried to acquire is not yet created "<<endl;
			return -1;

		}
		else
		{
			cout<<"\n !!!! Nachos  : The Lock reference received from the server is "<<recvdMsg<<endl;
			cout<<"\n !!!! Nachos  : The Acquire Lock is "<<recvdMsg<<endl;
			return lockRef;
		}
		
	#endif

	bool state;
	int ID=ref;
	if(ID<0 || ID >MaxLocksConditions) // CHECKING FOR VALID LOCK REFERENCE
	{
		printf("\n !!!! Nachos: Invalid Lock Reference. !!!! \n");
		return -1;
	}
	// YES LOCK REFERENCE IS VALID
	findLockBitMapMutex->Acquire();
	state=findLockBitMap->Test(ID);
	if(state) // CHECKING IF THE LOCK IS CREATED IN FIRST PLACE
	{
		// OK THE LOCK IS CREATED 
		if(lockAddrSpace[ID]==currentThread->space) // CHECKING IF THE LOCK IS BELONG TO THE CURRENT THREADS ADDR SPACE
		{
			findLockBitMapMutex->Release(); 
			// YES BOTH ARE IN THE SAME ADDRESS SPACE
			lockArr[ID]->Acquire(); // ACQUIRING LOCK 
			//printf("\n !!!! Nachos : Acquired Lock is %d . !!!!\n",ID);
			return 0;
		}
		else
		{
			findLockBitMapMutex->Release();
			// SORRY THE LOCK IS ASSOCIATED WITH A DIFFERENT ADDRESS SPACE
			printf("\n !!!! Nachos : The current Thread does not have permssion access this lock . !!!! \n");
			return -1;
		}
	}
	else
	{
			findLockBitMapMutex->Release();
			// SORRY THE LOCK ITSELF IS NOT CREATED 
			printf("\n !!!! Nachos : The lock with number %d is not yet created. !!!! \n",ID);
			return -1;

	}

}

/*-----------------------------------------------------------------------------
RELEASE LOCK SYSTEM CALL KERNEL CODE. WHEN CALLED RELEASES THE LOCK 
FOR THE USER PROGRAM. TAKES A INTEGER AS THE LOCK REFERENCE.
-----------------------------------------------------------------------------*/
int ReleaseLock_Syscall(int ref)
{

	#ifdef NETWORK
		PacketHeader outPktHdr, inPktHdr;
	    MailHeader outMailHdr, inMailHdr;
		char *lockNum=new char[32];
		char msgData[32]="103 ";
		sprintf(lockNum,"%d",ref);
		strcat(msgData,lockNum);
		
		// CONSTRUCTING THE MESSAGE HEADER
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgData) + 1;
		
		// TRYING TO SEND MESSAGE TO SERVER
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgData);  //Sending the data to the server to perform the task

		if ( !success ) // TRYING IF SEND IS SUCCESSFULL
		{
			cout<<"\n !!!! Nachos : Release Lock send request failed  !!!!"<<endl;			  
			interrupt->Halt();
		}

		char recvdMsg[32];
		postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
		int lockRef;
		lockRef=atoi(recvdMsg);
		if(lockRef<0)
		{
			cout<<"\n !!!! Nachos : The Lock that the process tried to Release is not yet created or this process is not the owner of the lock "<<endl;
			return -1;

		}
		else
		{
			cout<<"\n !!!! Nachos  : The Lock reference received from the server is "<<recvdMsg<<endl;
			cout<<"\n !!!! Nachos  : The Released Lock is "<<recvdMsg<<endl;
			return lockRef;
		}

		//return lockRef;
	#endif

	bool state;
	int ID=ref;
	if(ID<0 || ID >MaxLocksConditions) // CHECKING FOR VALID LOCK REFERENCE
	{
		// INVALID LOCK REFERENCE
		printf("\n !!!! Nachos: Invalid Lock Reference. !!!! \n");
		return -1;
	}

	findLockBitMapMutex->Acquire();
	state=findLockBitMap->Test(ID);
	if(state) // CHECKING IF THE LOCK IS CREATED FIRST
	{
		if(lockAddrSpace[ID]==currentThread->space) // CHECK IF THE THREAD CAN ACCESS THIS LOCK 
		{
			findLockBitMapMutex->Release(); 
			lockArr[ID]->Release(); // TRYING TO RELEASE THE LOCK 
			//printf("\n !!!! Nachos : Relased Lock is %d . !!!!\n",ID);

		}
		else
		{
			findLockBitMapMutex->Release();
			// SORRY THE LOCK IS IN DIFFERENT ADDRESS SPACE 
			printf("\n !!!! Nachos : The current thread does not have permission to access this lock . !!!! \n");
			return 0;
		}
	}
	else
	{ 
		// SORRY THE LOCK IS NOT YET CREATED 
		findLockBitMapMutex->Release();
		printf("\n!!!! Nachos : the lock with number %d is not yet created . !!!! \n",ID);
		return 0;
	}

}

/*-----------------------------------------------------------------------------
FORK SYSTEM CALL KERNEL CODE. WHEN CALLED FROM A USER PROGRAM. CREATE A NEW 
THREAD INSIDE THE SAME ADDRESS SPACE. TAKES JUST ONE PARAMETER WHICH IS THE 
VIRTUAL ADDRESS OF THE FUNCTION THAT WE ARE GOING TO EXECUTE
-----------------------------------------------------------------------------*/

void forkedKernelThread(int virtFuncAddr)
{

	//printf("\n Inside the Forked Kernel Thread function \n");
	currentThread->space->stackPageLock->Acquire(); // LOCKING THE STACK PAGE
	machine->WriteRegister(PCReg,virtFuncAddr); // WRITING PCREG
	machine->WriteRegister(NextPCReg,virtFuncAddr+4); // WRITING NEXT PC REG
	currentThread->space->RestoreState(); // RESTORING STATE
	currentThread->space->stackPage++; // GETTING THE STAKE PAGE
	unsigned int currentStackPageNumber=currentThread->space->numPages-((currentThread->space->stackPage)*divRoundUp(UserStackSize,PageSize));
	machine->WriteRegister(StackReg,(currentStackPageNumber*PageSize)-16);
	currentThread->space->stackPageLock->Release();
	machine->Run(); // CALLING MACHINE RUN

}
void Fork_Syscall(int virtFuncAddr)
{
	//printf("\n Inside Fork_Syscall...... \n");
	proTableLock->Acquire();
	int pnum=currentThread->space->processNumber; // GETTING THE PROCESS NUMBER 
	if(proTable[pnum].threadCount>MaxThreadCountPerProcess) // CHECKING IF MAXIMUM THREAD COUNT IS REACHED
	{
		// YES MAX THREAD COUNT REACHED
		printf("\n !!!! Nachos Limit : You have execeded the maximum number of threads per process .!!!!\n");
		printf("\n !!!! Machine Going To Halt ....  !!!!\n");
		proTableLock->Release();
		interrupt->Halt();
	}
	else
	{
		// NO MAX THREAD COUNT NOT YET REACHED
		virtFuncAddr=machine->ReadRegister(4); // READING THE NAME OF THE FUNCTION WHICH NEED TO BE FORKED
		Thread *forkedThread; 
		forkedThread=new Thread("forkedThread"); // CREATING A NEW KERNEL THREAD
		proTable[pnum].threadCount++; // UPDATING INFORMATION INTO THE PROCESS TABLE
		//printf("\nProcess Table : Process ID = %d , Thread Count=%d \n",proTable[pnum].processID,proTable[pnum].threadCount); // PRINTING OUT THE PROCESS TABLE INFORMARTION
		proTableLock->Release();
		forkedThread->space=currentThread->space; // ASSIGING THE KERNLE THREAD ADDR SPACE AS THE PROCESS ADDR SPACE
		forkedThread->Fork(forkedKernelThread,virtFuncAddr); // FORKING THE KERNEL THREAD
		//printf("\n Forking kernel Thread done \n");
	}

}


/*-----------------------------------------------------------------------------
EXEC SYSTEM CALL KERNEL CODE. WHEN CALLED CREATE A NEW PROCESS FOR THE USER 
PROGRAM. TAKES IN TWO PARAMETERS 1. A GORUP OF CHARACTERS THAT INDICATES 
THE PATH AND NAME OF THE EXECUTABLE, 2. THE LEGTH OF THE PARAMETER 1.
-----------------------------------------------------------------------------*/
void exec_Thread(int arg)
{
	 printf("\n process is created with ID %d",currentThread->space->processNumber);
	 currentThread->space->InitRegisters();    //Initialize Registers
     currentThread->space->RestoreState();     //Restore the state 
	 machine->Run();
}

int Exec_Syscall(int name, int nameLen)
{
	//printf("\n Inside Exec System Call\n");
	runningProcessCountLock->Acquire();
	if(runningProcessCount>=MaxProcessCount) // MCGS: CHECKING IF MAX PROCESS HAS REACHED 
	{
		// YES MAX PROCESS IS REACHED
		printf("\n !!!! Nachos Limit : You have execeded the maximum number of process .!!!!\n");
		printf("\n !!!!  Machine Going To Halt ....  !!!!\n");
		runningProcessCountLock->Release();
		interrupt->Halt();
		//return -1;
	}
	else
	{
		// NO MORE PROCESS CAN BE CREATED
		runningProcessCountLock->Release();
		name=machine->ReadRegister(4); // MCGS : READING THE NAME OF THE PATH OF THE EXE FILE
		nameLen=machine->ReadRegister(5); // MCGS : READING THE LENGHT OF THE EXE FILE PATH
		char exeName[nameLen+1];
		if(copyin(name,nameLen+1,exeName) == -1 ) // COPYING THE NAME OF THE EXECUTABLE
		{
			printf("\n !!!! Nachos : Bad pointer passed to Open !!!!\n ");
			return -1;
		}
		exeName[nameLen]='\0';
		OpenFile *exeFile = fileSystem->Open(exeName);// MCGS : OPENING THE EXECUTABLE FILE
		if (exeFile == NULL)
		{
			printf("\n !!!! Nachos : Unable to open the file !!!!\n");
			return -1;
		}
		AddrSpace *newSpace=new AddrSpace(exeFile); // MCGS : CREATING A NEW ADDR SPACE FOR THE EXE
		char *threadName=new char[20];
		sprintf(threadName,"execThread%d",newSpace->processNumber); // MCGS : A NAME FOR THE KERNEL THREAD IS CREATED

		///int pnum=newSpace->processNumber;

		Thread *execThread;
		execThread= new Thread(threadName); // MCGS : CREATING A KERNEL THREAD TO RUN THE MAIN METHOD IN THE EXC-ED PROCESS
		//printf("\n Thread created successfully \n");

		execThread->space = newSpace; // MCGS :ASSOCIATING THE KERNEL THREADS SPACE TO THE NEW SPACE THAT WAS CREATED UP
		//printf("\n Thread space changed successfully \n");

		// MCGS 3 :delete exeFile; // MCGS : CLOSING TH EXE FILE
		//printf("\n executable closed\n");

		execThread->Fork((VoidFunctionPtr)exec_Thread,2); // MCGS : FORKING THE KERNEL THREAD
		//printf("\n Fork calling Done\n");

		return (newSpace->processNumber); // MCGS : RETURNING THE PROCESS NUMBER
	}

}


/*-----------------------------------------------------------------------------
EXIT SYSTEM CALL KERNEL CODE . WHEN CALLED FROM THE USER PROGRAM FINISHED THE 
EXCUTION OF THE CURRENT THREAD
-----------------------------------------------------------------------------*/
void Exit_Syscall(int value)
{
	printf("\n Inside Exit system call\n");
	printf("\n the value got in exit system call is %d",value);
	
//	currentThread->Finish();
//	return;

	runningProcessCountLock->Acquire();
	if(runningProcessCount>1) // CHECKING IF THIS IS THE LAST PROCESS
	{
		// NOT THE LAST PROCESS
		int pnum=currentThread->space->processNumber;
		proTableLock->Acquire();
		if(proTable[pnum].threadCount>1) // CHECKING IF THIS IS THE LAST THREAD IN THIS PROCESS
		{
			// NOT THE LAST THREAD IN THIS PROCESS
			proTable[pnum].threadCount--; // DECEMENTING THE THREAD COUNT OF THIS PROCESS
			proTableLock->Release();
			runningProcessCountLock->Release();
			printf("\n !!!! Nachos : This is the not last thread int this process of nachos !!!! \n");
			printf("\n !!!! Nachos : Calling current thread finish  .... !!!! \n");
			currentThread->Finish();
		}
		else
		{
			// LAST THREAD IN THIS PROCESS
			printf("\n !!!! Nachos : This is the last thread int this process of nachos !!!! \n");
			runningProcessCount--; // DECREMENTING THE PROCESS COUNT 
			proTable[pnum].threadCount=0; // UPDATING THE THREAD COUNT IN PROCESS TABLE
			proTable[pnum].processID=-1; // UPDATING THE PROCESS NUMBER IN PROCESS TABLE
			proTableLock->Release();
			runningProcessCountLock->Release();
			/*MCGS :3 NOT SURE IF ITS CORRECT   int proNumPages=currentThread->space->numPages; // FINDING THE NUMBER PAGES ALLOCATED FOR THIS PROCESS
			int proStartPage=currentThread->space->pageTable[0].physicalPage; // FINDING THE STARTING PAGE
			int proEndPage=proStartPage+proNumPages; // CALCULATING THE ENDING PAGE
			physicalPageLock->Acquire(); // ACQUIRING THE LOCK TO THE MAIN MEMORY 
			printf("\n !!!! Nachos : De allocating main memory  !!!! \n");
			for(int i=proStartPage;i<proEndPage;i++)
			{
				findPhysicalPage->Clear(i); // CLEARING THE BIT MAP 
			}MCGS */
			processIdLock->Acquire();
			findProcessId->Clear(currentThread->space->processNumber);
			processIdLock->Release();
			//physicalPageLock->Release(); //RELEASING THE LOCK
			//delete(currentThread->space); // DELETING THE ADDRESS SPACE OF THE CURRENT PROCESS
			printf("\n !!!! Nachos : Calling current thread finish  !!!! \n");
			currentThread->Finish(); // CALLING CURRENT THREAD FINISH 
		}
	}
	else
	{
		// THE LAST PROCESS
		int pnum=currentThread->space->processNumber;
		proTableLock->Acquire();
		if(proTable[pnum].threadCount>1) // CHECKING IF THIS IS THE LAST THREAD IN THIS LAST PROCESS
		{

			// NOT THE LAST THREAD IN THE LAST PROCESS
			runningProcessCountLock->Release();
			proTable[pnum].threadCount--;
			proTableLock->Release();
			printf("\n !!!! Nachos : This is the not last thread int this last  process of nachos !!!! \n");
			printf("\n !!!! Nachos : Calling current thread finish  .... !!!! \n");
			currentThread->Finish();


		}
		else
		{
			//  THE LAST THREAD IN THE LAST PROCESS
			runningProcessCount--; // DECEMENTING THE RUNNING PROCESS
			runningProcessCountLock->Release();
			proTableLock->Release();
			printf("\n !!!! Nachos : This is the last thread in the last process of nachos !!!! \n");
			printf("\n !!!! Nachos : Going to halt the machine .... !!!! \n");
			interrupt->Halt();

		}

	}
	

}

/*-----------------------------------------------------------------------------
CREATE CONDITION SYSTEM CALL KERNEL CODE. WHEN CALLED RETURNS AN UNIQUE INTEGER
WHICH IS THE CONDITION REFERENCE TO THE USER PROGRAM
-----------------------------------------------------------------------------*/
int CreateCondition_Syscall(int dummyName,int nameLen)
{
	#ifdef NETWORK
		PacketHeader outPktHdr, inPktHdr;
	    MailHeader outMailHdr, inMailHdr;
		char *conditionName=new char[32];
		char msgData[32]="105";
		if( copyin(dummyName,nameLen,conditionName) == -1 ) 
		{
			cout<<"\n !!!!! Nachos Bad pointer passed to function !!!!"<<endl;
			return -1;
		}
		conditionName[nameLen]='\0';
		strcat(msgData,conditionName);
		
		// CONSTRUCTING THE MESSAGE HEADER
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgData) + 1;
		
		// TRYING TO SEND MESSAGE TO SERVER
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgData);  //Sending the data to the server to perform the task

		if ( !success ) // TRYING IF SEND IS SUCCESSFULL
		{
			cout<<"\n !!!! Nachos : Create Condtition send request failed  !!!!"<<endl;			  
			interrupt->Halt();
		}
		postOffice->Receive(1, &inPktHdr, &inMailHdr,msgData);
		cout<<"\n Nachos !!!! : The Condition reference received from the server is "<<msgData<<endl;
		cout<<"\n Nachos !!!! : The created condition is "<<msgData<<endl;
		int lockRef;
		lockRef=atoi(msgData);
		return lockRef;
	#endif

	//printf("\n Inside create condition sys call \n");
	int ID;
	char *name=new char[20];
	findConditionBitMapMutex->Acquire();
	ID=findConditionBitMap->Find(); // CHECKING IF MAX CONDITION IS REACHED
	if(ID==-1)
	{
		//YES MAX CONDITION REACHED
		printf("\n!!!! Nachos : You cannot create anymore condition. Max condition variable cout reached . !!!! \n");
		findConditionBitMapMutex->Release();
		return -1;
	}
	else
	{
		//NO MAX CONDITION NOT YET REACHED
		sprintf(name,"Condition#%d",ID);
		conditionArr[ID]=new Condition(name);// CREATING A NEW CONDTION VARIABLE
		conditionAddrSpace[ID]=currentThread->space;
		printf("\n !!!!! Nachos : A Condition Variable is created with ID : %d and is within the address space %u . !!!!\n",ID,currentThread->space);	
	}
	findConditionBitMapMutex->Release();
	return ID; // RETURNING THE REFERENCE TO THE CONDITION VARIABLE
}

/*------------------------------------------------------------------------------------
DESTROY CONDITION SYSTEM CALL KERNEL CODE. WHEN CALLED DESTROYS THE CONDITION VARIABLE.
TAKES A INTEGER AS THE CONDITION REFERENCE FROM THE USER PROGRAM.
-----------------------------------------------------------------------------*/
int DestroyCondition_Syscall(int ref)
{
		#ifdef NETWORK
			PacketHeader outPktHdr, inPktHdr;
			MailHeader outMailHdr, inMailHdr;
			char *conditionNum=new char[32];
			char msgData[32]="106 ";
			sprintf(conditionNum,"%d",ref);
			strcat(msgData,conditionNum);
			
			// CONSTRUCTING THE MESSAGE HEADER
			outPktHdr.to = 0;		
			outMailHdr.to = 0;
			outMailHdr.from = 1;
			outMailHdr.length = strlen(msgData) + 1;
			
			// TRYING TO SEND MESSAGE TO SERVER
			bool success = postOffice->Send(outPktHdr, outMailHdr, msgData);  //Sending the data to the server to perform the task

			if ( !success ) // TRYING IF SEND IS SUCCESSFULL
			{
				cout<<"\n !!!! Nachos : Destroy Condition send request failed  !!!!"<<endl;			  
				interrupt->Halt();
			}
			else
			{
				cout<<"\n  !!!! Nachos :Condition Destroy Request message succesfully sent to server !!!!"<<endl;
			}
			char recvdMsg[32];
			postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
			int conditionRef;
			conditionRef=atoi(recvdMsg);
			if(conditionRef<0)
			{
				cout<<"\n!!!! Nachos : Cannot destroy condition since the condition variable is not yet created or the wait queue is not empty. "<<endl;
				return -1;
			}
			else
			{
				cout<<"\n !!!! Nachos : The Condition reference received from the server is "<<recvdMsg<<endl;
				cout<<"\n !!!! Nachos : The destroyed Condition is "<<recvdMsg<<endl;
			}
			return conditionRef;
		#endif


	//printf("\n Inside destroy condition sys call \n");
	bool state;
	int conditionID=ref;
	if(conditionID < 0 || conditionID > MaxLocksConditions)
	{
		printf("\n !!!! Nachos : Invalid condition variable indentifier or lock identifier. !!!! \n");
		return -1;
	}
	findConditionBitMapMutex->Acquire();
	state=findConditionBitMap->Test(conditionID);
	if(state) // CHECKING IF THE CV IS CREATED
	{
		if(conditionAddrSpace[conditionID]==currentThread->space) // CHECKING IF THE CV IS ASSOCIATED WITH CURRENT THREADS ADDR SPACE
		{
			if(conditionArr[conditionID]->conditionQueue->IsEmpty()) // CHECKING IF THE CONDITION QUEUE IS EMPTY
			{
				// CONDITION QUEUE IS EMPTY
				findConditionBitMap->Clear(conditionID); // CLEARING THE BIT MAP
				delete(conditionArr[conditionID]); // DESTROYING THE CONDITION VARIABLE
				conditionAddrSpace[conditionID]=NULL; // CLEARING THE ADDRESS SPACE ASSOCIATION
				printf("\n !!!! Nachos : The destroyed condition is %d !!!! \n",conditionID);
				findConditionBitMapMutex->Release();
				return 0;
			}
			else
			{
				// CONDITION QUEUE IS NOT EMPTY SO CANNOT DESTROY CONDITION
				findConditionBitMapMutex->Release();
				printf("\n !!!! Nachos : Cannot destroy condition since one or more thread(s) is  waiting on this condition. !!!! \n");
				return -1;
			}
		}
		else
		{
			// CONDITION VARIABLE IS ASSOCIATED WITH DIFFERENT ADDRESS SPACE
			findConditionBitMapMutex->Release();
			printf("\n !!!! Nachos : current thread does not have permission to access this condition variable. !!!! \n");
			return -1;
		}
	}
	else
	{
		// CONDITION VARIABLE IS NOT YET CREATED 
		findConditionBitMapMutex->Release();
		printf("\n !!!! Nachos : The condition variable %d is not yet created. !!!! \n",conditionID);
		return -1;
	}
}

/*------------------------------------------------------------------------------------
WAIT CONDITION SYSTEM CALL KERNEL CODE. WHEN CALLED MAKES THE CALLING THREAD WAIT 
ON THIS CONDITION VARIABLE. TAKES TWO PARAMETERS 1. IS REFERENCE OF THE CONDTION 
VARIABLE 2. THE REFERENCE OF THE LOCK 
--------------------------------------------------------------------------------------*/
int WaitCondition_Syscall(int CondRef, int LockRef)
{
	#ifdef NETWORK

		PacketHeader outPktHdr, inPktHdr;
		MailHeader outMailHdr, inMailHdr;
		char *msgPart1,*msgPart2;
		msgPart1=new char[32];
		msgPart2=new char[32];

		char msgToSend[32]="107";  //Identifying Wait system call
		strcat(msgToSend," ");
	  	sprintf(msgPart1,"%d",CondRef);
		strcat(msgToSend,msgPart1);  
		strcat(msgToSend," ");
		sprintf(msgPart2,"%d",LockRef);
		strcat(msgToSend,msgPart2);
		 
		// CONSTRUCTING PACKET
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgToSend) + 1;

		// SENDING MESSAGE
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgToSend); //Sending the data to the server to perform the task
		if ( !success ) 
		{
			cout<<"\n!!!! Nachos Error: Wait send Request failed. !!!!!"<<endl;
			interrupt->Halt();
		}
		// Wait for the first message from the other machine
		char recvdMsg[32];
		postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
		cout<<"\n The received msg is "<<recvdMsg<<endl;
		int conditionRef;
		conditionRef=atoi(recvdMsg);
		if(conditionRef<0)
		{
			cout<<"\n!!!! Nachos : Condition wait failed "<<endl;
			return -1;
		}
		return conditionRef;


	#endif

	//printf("\n Inside wait condition sys call \n");
	int conditionID=CondRef;
	int lockID=LockRef;
	bool conditionState;
	bool lockState;
	if( conditionID < 0 || lockID < 0 || conditionID > MaxLocksConditions || lockID > MaxLocksConditions)
	{
		printf("\n !!!! Nachos : Invalid condition variable indentifier or lock identifier. !!!! \n");
		return -1;
	}
	findConditionBitMapMutex->Acquire();
	conditionState=findConditionBitMap->Test(conditionID);
	findConditionBitMapMutex->Release();

	findLockBitMapMutex->Acquire();
	lockState=findLockBitMap->Test(lockID);
	findLockBitMapMutex->Release();
	if(conditionState && lockState)
	{
		// BOTH THE LOCK AND CONDTITION VARIABLE ARE CREATED
		if(conditionAddrSpace[conditionID]==currentThread->space) // CHECKING IF THE CONDITION VARIABLE IS IN THE SAME ADDRESS SPACE
		{
			// YES THE CONDTION VARIABLE IS IN THE SAME ADDRESS SPACE
			if(lockAddrSpace[lockID]==currentThread->space) // CHECKING IF THE LOCK IS IN THE SAME ADDRESS SPACE
			{
				// YES THE LOCK IS ALSO WITH IN THE SAME ADDRESS SPACE
				//printf("\n !!!! Nachos : Wait  called .... !!!! \n");
				conditionArr[conditionID]->Wait(lockArr[lockID]); // CALLING WAIT
				return 0;
			}
			else
			{
				// NO THE LOCK IS NOT IN THE SAME ADDRESS SPACE
				printf("\n !!!! Nachos: Current thread does not have permission to access this lock. !!!! \n");
				return -1;
			}
		}
		else
		{
			// NO THE CONDITION VARIABLE IS IN NOT THE SAME ADDRESS SPACE
			printf("\n !!!! Nachos: Current thread does not have permission to access this condition variable. !!!! \n");
			return -1;
		}

	}
	else
	{
		// EITHER OF THE ONE IS NOT YET CREATED
		printf("\n !!!! Nachos <WAIT> : Either the lock or condition variable is not yet created. !!!! \n");
		return -1;
	}
}
/*--------------------------------------------------------------------------------------
SIGNAL CONDITION SYSTEM CALL KERNEL CODE. WHEN CALLED MAKES THE CALLING THREAD TO SIGNAL 
ON THIS CONDITION VARIABLE. TAKES TWO PARAMETERS 1. IS REFERENCE OF THE CONDTION 
VARIABLE 2. THE REFERENCE OF THE LOCK 
--------------------------------------------------------------------------------------*/
int SignalCondition_Syscall(int CondRef, int LockRef)
{
	#ifdef NETWORK

		PacketHeader outPktHdr, inPktHdr;
		MailHeader outMailHdr, inMailHdr;
		char *msgPart1,*msgPart2;
		msgPart1=new char[32];
		msgPart2=new char[32];

		char msgToSend[32]="108";  //Identifying Wait system call
		strcat(msgToSend," ");
	  	sprintf(msgPart1,"%d",CondRef);
		strcat(msgToSend,msgPart1);  
		strcat(msgToSend," ");
		sprintf(msgPart2,"%d",LockRef);
		strcat(msgToSend,msgPart2);
		 
		// CONSTRUCTING PACKET
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgToSend) + 1;

		// SENDING MESSAGE
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgToSend); //Sending the data to the server to perform the task
		if ( !success ) 
		{
			cout<<"\n!!!! Nachos Error: Signal send Request failed. !!!!!"<<endl;
			interrupt->Halt();
		}
		// Wait for the first message from the other machine
		char recvdMsg[32];
		postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
		int conditionRef;
		conditionRef=atoi(recvdMsg);
		cout<<"\n The received msg is "<<recvdMsg<<endl;
		if(conditionRef<0)
		{
			cout<<"\n!!!! Nachos : Condition Signal Failed "<<endl;
			return -1;
		}
		else
		{
			return conditionRef;
		}

	

	#endif


	//printf("\n Inside signal condition sys call \n");
	int conditionID=CondRef;
	int lockID=LockRef;
	bool conditionState;
	bool lockState;
	if( conditionID < 0 || lockID < 0 || conditionID > MaxLocksConditions || lockID > MaxLocksConditions)
	{
		printf("\n !!!! Nachos : Invalid condition variable indentifier or lock identifier. !!!! \n");
		return -1;
	}
	findConditionBitMapMutex->Acquire();
	conditionState=findConditionBitMap->Test(conditionID);
	findConditionBitMapMutex->Release();

	findLockBitMapMutex->Acquire();
	lockState=findLockBitMap->Test(lockID);
	findLockBitMapMutex->Release();
	if(conditionState && lockState)
	{
		// BOTH THE LOCK AND CONDTITION VARIABLE ARE CREATED
		if(conditionAddrSpace[conditionID]==currentThread->space) // CHECKING IF THE CONDITION VARIABLE IS IN THE SAME ADDRESS SPACE
		{
			// YES THE CONDTION VARIABLE IS IN THE SAME ADDRESS SPACE
			if(lockAddrSpace[lockID]==currentThread->space) // CHECKING IF THE LOCK IS IN THE SAME ADDRESS SPACE
			{
				// YES THE LOCK IS ALSO WITH IN THE SAME ADDRESS SPACE
				conditionArr[conditionID]->Signal(lockArr[lockID]); // CALLING SIGNAL
				//printf("\n !!!! NACHOS: Signal called !!!! \n ");
				return 0;
			}
			else
			{
				// NO THE LOCK IS NOT IN THE SAME ADDRESS SPACE
				printf("\n !!!! Nachos: Current thread does not have permission to access this lock. !!!! \n");
				return -1;
			}
		}
		else
		{
			// NO THE CONDITION VARIABLE IS IN NOT THE SAME ADDRESS SPACE
			printf("\n !!!! Nachos: Current thread does not have permission to access this condition variable. !!!! \n");
			return -1;
		}

	}
	else
	{
		// EITHER OF THE ONE IS NOT YET CREATED
		printf("\n !!!! Nachos <SIGNAL> : Either the lock or condition variable is not yet created. !!!! \n");
		return -1;
	}
}

/*--------------------------------------------------------------------------------------------
BROADCAST CONDITION SYSTEM CALL KERNEL CODE. WHEN CALLED MAKES THE CALLING THREAD TO BROADCAST 
ON THIS CONDITION VARIABLE. TAKES TWO PARAMETERS 1. IS REFERENCE OF THE CONDTION 
VARIABLE 2. THE REFERENCE OF THE LOCK 
--------------------------------------------------------------------------------------*/
int BroadcastCondition_Syscall(int CondRef, int LockRef)
{
	#ifdef NETWORK

		PacketHeader outPktHdr, inPktHdr;
		MailHeader outMailHdr, inMailHdr;
		char *msgPart1,*msgPart2;
		msgPart1=new char[32];
		msgPart2=new char[32];

		char msgToSend[32]="109";  //Identifying Wait system call
		strcat(msgToSend," ");
	  	sprintf(msgPart1,"%d",CondRef);
		strcat(msgToSend,msgPart1);  
		strcat(msgToSend," ");
		sprintf(msgPart2,"%d",LockRef);
		strcat(msgToSend,msgPart2);
		 
		// CONSTRUCTING PACKET
		outPktHdr.to = 0;		
		outMailHdr.to = 0;
		outMailHdr.from = 1;
		outMailHdr.length = strlen(msgToSend) + 1;

		// SENDING MESSAGE
		bool success = postOffice->Send(outPktHdr, outMailHdr, msgToSend); //Sending the data to the server to perform the task
		if ( !success ) 
		{
			cout<<"\n!!!! Nachos Error: BroadCast send Request failed. !!!!!"<<endl;
			interrupt->Halt();
		}
		// Wait for the first message from the other machine
		char recvdMsg[32];
		postOffice->Receive(1, &inPktHdr, &inMailHdr,recvdMsg);
		cout<<"\n The received msg is "<<recvdMsg<<endl;
		int conditionRef;
		conditionRef=atoi(recvdMsg);
		if(conditionRef<0)
		{
			cout<<"\n!!!! Nachos : Condition Broadcast Failed "<<endl;
			return -1;
		}
		else
		{
			return conditionRef;
		}

	

	#endif


	//printf("\n Inside broadcast condition sys call \n");
	int conditionID=CondRef;
	int lockID=LockRef;
	bool conditionState;
	bool lockState;
	if( conditionID < 0 || lockID < 0 || conditionID > MaxLocksConditions || lockID > MaxLocksConditions)
	{
		printf("\n !!!! Nachos : Invalid condition variable indentifier or lock identifier. !!!! \n");
		return -1;
	}
	findConditionBitMapMutex->Acquire();
	conditionState=findConditionBitMap->Test(conditionID);
	findConditionBitMapMutex->Release();

	findLockBitMapMutex->Acquire();
	lockState=findLockBitMap->Test(lockID);
	findLockBitMapMutex->Release();
	if(conditionState && lockState)
	{
		// BOTH THE LOCK AND CONDTITION VARIABLE ARE CREATED
		if(conditionAddrSpace[conditionID]==currentThread->space) // CHECKING IF THE CONDITION VARIABLE IS IN THE SAME ADDRESS SPACE
		{
			// YES THE CONDTION VARIABLE IS IN THE SAME ADDRESS SPACE
			if(lockAddrSpace[lockID]==currentThread->space) // CHECKING IF THE LOCK IS IN THE SAME ADDRESS SPACE
			{
				// YES THE LOCK IS ALSO WITH IN THE SAME ADDRESS SPACE
				conditionArr[conditionID]->Broadcast(lockArr[lockID]); // CALLING SIGNAL
				//printf("\n !!!! NACHOS: BroadCast called !!!! \n ");
				return 0;
			}
			else
			{
				// NO THE LOCK IS NOT IN THE SAME ADDRESS SPACE
				printf("\n !!!! Nachos: Current thread does not have permission to access this lock. !!!! \n");
				return-1;
			}
		}
		else
		{
			// NO THE CONDITION VARIABLE IS IN NOT THE SAME ADDRESS SPACE
			printf("\n !!!! Nachos: Current thread does not have permission to access this condition variable. !!!! \n");
			return -1;
		}

	}
	else
	{
		// EITHER OF THE ONE IS NOT YET CREATED
		printf("\n !!!! Nachos <BORADCAST>: Either the lock or condition variable is not yet created. !!!! \n");
		return -1;
	}
}


/*--------------------------------------------------------------------------------------------
MCGS 3: HandlePageFault(int badVirtAddr) . 
		A FUNCITON TO HANDLE THE PAGE FAULT EXCEPTION
		TAKES THE THE BAD VIRTUAL ADDRESS AS THE ARGUEMENT.
--------------------------------------------------------------------------------------*/

void HandlePageFault(int badVirtAddr)
{
	AddrSpace *addrSpacePtr;
	//IntStatus oldLevel=interrupt->SetLevel(IntOff);		// DISABLING THE INTERRUPT
	physicalPageLock->Acquire();							// ACQUIRING THE LOCK TO RAM BITMAP;
	int badVirtPageNumber=(badVirtAddr/PageSize);			// CALCULATING THE BAD VIRTUAL PAGE NUMBER
	int iptIndex=-1;										// A FALG VARIABLE TO MONITOR IPT HITS
	int tlbIndex=currentTLB%TLBSize;						// CALCULATING THE INDEX IN THE TLB

	IntStatus oldLevel=interrupt->SetLevel(IntOff);			// DISABLING THE INTERRUPT
	iptLock->Acquire();							
	if((machine->tlb[tlbIndex].dirty==TRUE) && (machine->tlb[tlbIndex].valid==TRUE))	// CHECKING IF THE SELECTED TLB ENTRY IS DIRTY
	{
		 int physPage=machine->tlb[tlbIndex].physicalPage;
		 int virtPage=machine->tlb[tlbIndex].virtualPage;
		 ipt[physPage].dirty=machine->tlb[tlbIndex].dirty;								// PROPAGATING THE DIRTY BITS TO THE IPT
		 currentThread->space->pageTable[virtPage].dirty=machine->tlb[tlbIndex].dirty;	// PROPAGATING THE DIRTY BITS TO THE PAGE TABLE
	}
	iptLock->Release();

	
	iptLock->Acquire();
	for(int i=0;i<NumPhysPages;i++) // SEARCHING THE IPT
	{
		if(currentThread->space==ipt[i].pSpace && badVirtPageNumber==ipt[i].virtualPage)
		{
			iptIndex=i; // CHEKCING IF THE PAGE IS THERE IN THE IPT;
		}
	}
	iptLock->Release();
	(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT
	if(iptIndex>=0) // FOUND IN IPT
	{

			oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERRUPT
			// UPDATING THE TLB FROM THE IPT
			machine->tlb[tlbIndex].virtualPage = ipt[iptIndex].virtualPage; 
			machine->tlb[tlbIndex].physicalPage = ipt[iptIndex].physicalPage; 
			machine->tlb[tlbIndex].valid = ipt[iptIndex].valid; 
			machine->tlb[tlbIndex].use = ipt[iptIndex].use;
			machine->tlb[tlbIndex].dirty = ipt[iptIndex].dirty;
			machine->tlb[tlbIndex].readOnly = ipt[iptIndex].readOnly;
			(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT
			
	}
	else // WE HAVE AN IPT MISS
	{
		unsigned int toPage,fromPage;
		int freePageNum;
		freePageNum=findPhysicalPage->Find(); // FINDING A FREE PAGE IN MEMORY
		if(freePageNum<0) // CHECKING IF THE RAM IS FULL
		{
			unsigned int pageToEvict;
			if(replacePolicy==RAND) // DECIDING ON THE PAGE REPLACEMENT POLICY
			{
				// THE PAGE REPLACEMENT POLICY IS RANDOM
				pageToEvict=rand()%NumPhysPages;
			}
			else
			{
				// THE PAGE REPLACEMENT POLICY IS FIFO
				fifoPage++;
				pageToEvict=fifoPage%NumPhysPages;

			}

			freePageNum=pageToEvict;
			//cout<<"\nPage selected for eviction is "<<pageToEvict<<endl;

			oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERRUPT
			// CHECKING IF THE PAGE SELECTED FOR EVICTION IN IN TLB
			for(int i=0;i<TLBSize;i++)
			{
				if(machine->tlb[i].physicalPage==pageToEvict)// YES THE PAGE SELECTED FOR EVICTION IS IN TLB
				{
					if(machine->tlb[i].dirty==TRUE)			// CHECKING IF THE PAGE SELCTED FOR EVICTION IS DIRTY ??
					{
						// YES THE PAGE SELCTED FOR EVICTION IS DIRTY SO SETTING THE CORRESPONDING IPT ENTRY TO DIRTY
						ipt[pageToEvict].dirty=TRUE;
					}
					machine->tlb[i].valid=FALSE; // INVALIDATING THE TLB ENTRY
				}
			}
			(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT
			
			if(ipt[pageToEvict].dirty==TRUE) // CHECKING IF THE PAGE SELECTED FOR EVICTION IS DIRTY OR NOT
			{
				// YES THE PAGE IS DIRTY
				int pageInSwapFile=swapfileBitMap->Find();
				if(pageInSwapFile==-1)
				{
					// CALLING INTERRUPT HALT IF THE SWAP FILE IS FULL
					cout<<"\n Swap File is full"<<endl;
					interrupt->Halt();
				}
				// WRITING PAGE TO SWAP FILE
				swapfile->WriteAt(&(machine->mainMemory[pageToEvict * PageSize]),PageSize,(pageInSwapFile * PageSize));
				//cout<<"\n evicted page Written to swapfile"<<endl;

				// UPDATING THE PAGE TABLE FOR THIS EVICTED PAGE
				addrSpacePtr=ipt[pageToEvict].pSpace;
				addrSpacePtr->pageTable[ipt[pageToEvict].virtualPage].pageLocation=SWAP; 
				addrSpacePtr->pageTable[ipt[pageToEvict].virtualPage].diskLocation=pageInSwapFile * PageSize;
				//cout<<"\n Page Table updated for the evicted page"<<endl;

			}
			else
			{
				//cout<<"\nThe page selected for eviction is not dirty"<<endl;
			}
			
			//cout<<"\n bad virt addr"<<badVirtAddr<<endl;
			//cout<<"\n bad virt page number is "<<badVirtPageNumber<<endl;
			
			// CHECKING IF THE PAGE TO BE LOADED IS IN THE EXE
			if(currentThread->space->pageTable[badVirtPageNumber].pageLocation==EXE)
			{
				//cout<<"\n IPT MISS N EXE **************The page to be loaded is in exe"<<endl;
				int pnum=currentThread->space->processNumber;
				proTable[pnum].exeHandle->ReadAt(&(machine->mainMemory[freePageNum*PageSize]),PageSize,currentThread->space->pageTable[badVirtPageNumber].diskLocation); // NOW THE PAGE IS LOADED IN TO MAIN MEMORY
				//cout<<"\n IPT MISS N EXE ++++++++++++++the page which faluted is loaded into memory"<<endl;
				//ipt[freePageNum].dirty=FALSE;//currentThread->space->pageTable[badVirtPageNumber].dirty;
				
			}
			else if(currentThread->space->pageTable[badVirtPageNumber].pageLocation==SWAP) // CHEKING IF IS IT IN SWAP
			{
				//cout<<"\n IPT MISS N SWAP ************The page to be loaded is in swapfile"<<endl;
				swapfile->ReadAt(&(machine->mainMemory[freePageNum * PageSize]),PageSize,currentThread->space->pageTable[badVirtPageNumber].diskLocation);
				//cout<<"\n IPT MISS N SWAP ++++++++++++the page which faluted is loaded into memory"<<endl;
				//ipt[freePageNum].dirty=FALSE; // currentThread->space->pageTable[badVirtPageNumber].dirty;
			}

				//UPDATING THE PAGE TABLE
				currentThread->space->pageTable[badVirtPageNumber].physicalPage=freePageNum;
				currentThread->space->pageTable[badVirtPageNumber].dirty=FALSE;
				//cout<<"\n Page Table updated for the newly loaded page"<<endl;

				
				// UPDATING THE IPT
				iptLock->Acquire();
				ipt[freePageNum].physicalPage=freePageNum;
				ipt[freePageNum].virtualPage=badVirtPageNumber;
				ipt[freePageNum].valid = currentThread->space->pageTable[badVirtPageNumber].valid;
				ipt[freePageNum].use = currentThread->space->pageTable[badVirtPageNumber].use;
				ipt[freePageNum].dirty = FALSE;
				ipt[freePageNum].readOnly = currentThread->space->pageTable[badVirtPageNumber].readOnly;
				ipt[freePageNum].pid = currentThread->space->processNumber;
				ipt[freePageNum].pSpace=currentThread->space;
				//cout<<"\n ipt updated for the newly loaded page"<<endl;
				iptLock->Release();

				
				// UPDATING THE TLB
				oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERRUPT
				machine->tlb[tlbIndex].virtualPage = ipt[freePageNum].virtualPage; // COPYING THE VIRTUAL PAGE NUMBER
				machine->tlb[tlbIndex].physicalPage = ipt[freePageNum].physicalPage; // COPYING THE PHYSICAL PAGE NUMBER
				machine->tlb[tlbIndex].valid = ipt[freePageNum].valid; 
				machine->tlb[tlbIndex].use = ipt[freePageNum].use;
				machine->tlb[tlbIndex].dirty = ipt[freePageNum].dirty;
				machine->tlb[tlbIndex].readOnly = ipt[freePageNum].readOnly;
				//cout<<"\n tlb updated for newly loaded page"<<endl;
				(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT

			

				//physicalPageLock->Release(); // ACQUIRING THE LOCK TO RAM BITMAP;
		}
		else // RAM IS NOT FULL
		{
				//cout<<"\n RAM is not full"<<endl;
				int pnum=currentThread->space->processNumber;

				// CHECKING IF THE PAGE TO BE LOADED IS IN THE EXE
				if(currentThread->space->pageTable[badVirtPageNumber].pageLocation==EXE)
				{
					//cout<<"\n RAM NOT FULL N EXE**************The page to be loaded is in exe"<<endl;
					//int pnum=currentThread->space->processNumber;
					proTable[pnum].exeHandle->ReadAt(&(machine->mainMemory[freePageNum*PageSize]),PageSize,currentThread->space->pageTable[badVirtPageNumber].diskLocation); // NOW THE PAGE IS LOADED IN TO MAIN MEMORY
					//cout<<"\n  RAM NOT FULL N EXE++++++++++++++the page which faluted is loaded into memory"<<endl;
					ipt[freePageNum].dirty=FALSE;//currentThread->space->pageTable[badVirtPageNumber].dirty;
					
				}
				else if(currentThread->space->pageTable[badVirtPageNumber].pageLocation==SWAP) // CHEKING IF IS IT IN SWAP
				{
					//cout<<"\n  RAM NOT FULL N SWAP************The page to be loaded is in swapfile"<<endl;
					swapfile->ReadAt(&(machine->mainMemory[freePageNum * PageSize]),PageSize,currentThread->space->pageTable[badVirtPageNumber].diskLocation);
					//cout<<"\n  RAM NOT FULL N SWAP++++++++++++the page which faluted is loaded into memory"<<endl;
					ipt[freePageNum].dirty=FALSE; // currentThread->space->pageTable[badVirtPageNumber].dirty;
				}

				//UPDATING THE PAGE TABLE
				currentThread->space->pageTable[badVirtPageNumber].physicalPage=freePageNum;
				currentThread->space->pageTable[badVirtPageNumber].dirty=FALSE;
				//cout<<"\n Page Table updated for the newly loaded page"<<endl;


				// UPDATING THE IPT 
				iptLock->Acquire();
				ipt[freePageNum].physicalPage=freePageNum;
				ipt[freePageNum].virtualPage=currentThread->space->pageTable[badVirtPageNumber].virtualPage;
				ipt[freePageNum].valid = currentThread->space->pageTable[badVirtPageNumber].valid;
				ipt[freePageNum].use = currentThread->space->pageTable[badVirtPageNumber].use;
				ipt[freePageNum].dirty = FALSE;
				ipt[freePageNum].readOnly = currentThread->space->pageTable[badVirtPageNumber].readOnly;
				ipt[freePageNum].pid = pnum;
				ipt[freePageNum].pSpace=currentThread->space;
				iptLock->Release();


				// UPDATING THE TLB
				oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERRUPT
				machine->tlb[tlbIndex].virtualPage = ipt[freePageNum].virtualPage; // COPYING THE VIRTUAL PAGE NUMBER
				machine->tlb[tlbIndex].physicalPage = ipt[freePageNum].physicalPage; // COPYING THE PHYSICAL PAGE NUMBER
				machine->tlb[tlbIndex].valid = ipt[freePageNum].valid; 
				machine->tlb[tlbIndex].use = ipt[freePageNum].use;
				machine->tlb[tlbIndex].dirty = ipt[freePageNum].dirty;
				machine->tlb[tlbIndex].readOnly = ipt[freePageNum].readOnly;
				(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT

				//physicalPageLock->Release(); // ACQUIRING THE LOCK TO RAM BITMAP;
		}
	}
	
	currentTLB++;
	//(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT
	physicalPageLock->Release(); // ACQUIRING THE LOCK TO RAM BITMAP;

}










// ----------------------------  END MCGS ---------------------------------------------------------- //

void ExceptionHandler(ExceptionType which) {
    int type = machine->ReadRegister(2); // Which syscall?
    int rv=0; 	// the return value from a syscall

    if(which == PageFaultException)
	{
		// CODE TO HANDLE PAGE FAULT EXCEPTION
		//printf("\n !!!! Nachos : Page Fault Exception Caught. !!!!");
		//IntStatus oldLevel=interrupt->SetLevel(IntOff); // DISABLING THE INTERRUPT
		HandlePageFault(machine->ReadRegister(BadVAddrReg));		// MCGS 3: CALLING A FUNCTION TO HANDLE PAGE FAULT EXCEPTION
		//(void) interrupt->SetLevel(oldLevel);  // RESTORING THE INTERRUPT
		return;
	}
	else if ( which == SyscallException ) 
	{
	switch (type) {
			
	    default:
		DEBUG('a', "Unknown syscall - shutting down.\n");

	    case SC_Halt:
		DEBUG('a', "Shutdown, initiated by user program.\n");
		interrupt->Halt();
		break;
	    case SC_Create:
		DEBUG('a', "Create syscall.\n");
		Create_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
		break;
	    case SC_Open:
		DEBUG('a', "Open syscall.\n");
		rv = Open_Syscall(machine->ReadRegister(4), machine->ReadRegister(5));
		break;
	    case SC_Write:
		DEBUG('a', "Write syscall.\n");
		Write_Syscall(machine->ReadRegister(4),
			      machine->ReadRegister(5),
			      machine->ReadRegister(6));
		break;
	    case SC_Read:
		DEBUG('a', "Read syscall.\n");
		rv = Read_Syscall(machine->ReadRegister(4),
			      machine->ReadRegister(5),
			      machine->ReadRegister(6));
		break;
	    case SC_Close:
		DEBUG('a', "Close syscall.\n");
		Close_Syscall(machine->ReadRegister(4));
		break;
	#ifdef CHANGED

//--------------------- MCGS ---------------------------------------------------------//
	    case SC_Yield:
	    	DEBUG('a', "Yield syscall.\n");
		Yield_Syscall();
		break;

		case SC_CreateLock:
				    	DEBUG('a', "Create Lock syscall.\n");
		rv=CreateLock_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
		break;
		
		case SC_DestroyLock:
				    	DEBUG('a', "Destroy Lock syscall.\n");
		DestroyLock_Syscall(machine->ReadRegister(4));
		break;

		case SC_AcquireLock:
				    	DEBUG('a', "Acquire Lock syscall.\n");
		AcquireLock_Syscall(machine->ReadRegister(4));
		break;

		case SC_ReleaseLock:
				    	DEBUG('a', "Release Lock syscall.\n");
		ReleaseLock_Syscall(machine->ReadRegister(4));
		break;

		case SC_Exec:
				    	DEBUG('a', "Exec  syscall.\n");
		rv=Exec_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
		break;

		case SC_Exit:
				    	DEBUG('a', "Exit  syscall.\n");
		Exit_Syscall(machine->ReadRegister(4));
		rv=-1;
		break;

		case SC_Fork:
				    	DEBUG('a', "Fork  syscall.\n");
		Fork_Syscall(machine->ReadRegister(4));
		rv=-1;
		break;

		case SC_CreateCondition:
				    	DEBUG('a', "Create Condition  syscall.\n");
		rv=CreateCondition_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
		break;

		case SC_DestroyCondition:
				    	DEBUG('a', "Destroy Condition  syscall.\n");
		DestroyCondition_Syscall(machine->ReadRegister(4));
		rv=-1;
		break;

		case SC_WaitCondition:
				    	DEBUG('a', "Wait Condition  syscall.\n");
		WaitCondition_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
		rv=-1;
		break;

		case SC_SignalCondition:
				    	DEBUG('a', "Signal Condition  syscall.\n");
		SignalCondition_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
		rv=-1;
		break;

		case SC_BroadcastCondition:
				    	DEBUG('a', "Broadcasr Condition  syscall.\n");
		BroadcastCondition_Syscall(machine->ReadRegister(4),machine->ReadRegister(5));
		rv=-1;
		break;

//--------------------- END OF MCGS ---------------------------------------------------------//

	#endif
	}

	// Put in the return value and increment the PC
	machine->WriteRegister(2,rv);
	machine->WriteRegister(PrevPCReg,machine->ReadRegister(PCReg));
	machine->WriteRegister(PCReg,machine->ReadRegister(NextPCReg));
	machine->WriteRegister(NextPCReg,machine->ReadRegister(PCReg)+4);
	return;
    }
	else 
	{
      cout<<"Unexpected user mode exception - which:"<<which<<"  type:"<< type<<endl;
      interrupt->Halt();
    }
}
