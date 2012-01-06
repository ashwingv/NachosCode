// nettest.cc 
//	Test out message delivery between two "Nachos" machines,
//	using the Post Office to coordinate delivery.
//
//	Two caveats:
//	  1. Two copies of Nachos must be running, with machine ID's 0 and 1:
//		./nachos -m 0 -o 1 &
//		./nachos -m 1 -o 0 &
//
//	  2. You need an implementation of condition variables,
//	     which is *not* provided as part of the baseline threads 
//	     implementation.  The Post Office won't work without
//	     a correct implementation of condition variables.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "network.h"
#include "post.h"
#include "interrupt.h"
#include <iostream.h>

// Test out message delivery, by doing the following:
//	1. send a message to the machine with ID "farAddr", at mail box #0
//	2. wait for the other machine's message to arrive (in our mailbox #0)
//	3. send an acknowledgment for the other machine's message
//	4. wait for an acknowledgement from the other machine to our 
//	    original message

void
MailTest(int farAddr)
{
    PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
    char *data = "Hello there!";
    char *ack = "Got it!";
    char buffer[MaxMailSize];

    // construct packet, mail header for original message
    // To: destination machine, mailbox 0
    // From: our machine, reply to: mailbox 1
    outPktHdr.to = farAddr;		
    outMailHdr.to = 0;
    outMailHdr.from = 1;
    outMailHdr.length = strlen(data) + 1;

    // Send the first message
    bool success = postOffice->Send(outPktHdr, outMailHdr, data); 

    if ( !success ) {
      printf("The postOffice Send failed. You must not have the other Nachos running. Terminating Nachos.\n");
      interrupt->Halt();
    }

    // Wait for the first message from the other machine
    postOffice->Receive(0, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Send acknowledgement to the other machine (using "reply to" mailbox
    // in the message that just arrived
    outPktHdr.to = inPktHdr.from;
    outMailHdr.to = inMailHdr.from;
    outMailHdr.length = strlen(ack) + 1;
    success = postOffice->Send(outPktHdr, outMailHdr, ack); 

    if ( !success ) {
      printf("The postOffice Send failed. You must not have the other Nachos running. Terminating Nachos.\n");
      interrupt->Halt();
    }

    // Wait for the ack from the other machine to the first message we sent.
    postOffice->Receive(1, &inPktHdr, &inMailHdr, buffer);
    printf("Got \"%s\" from %d, box %d\n",buffer,inPktHdr.from,inMailHdr.from);
    fflush(stdout);

    // Then we're done!
    interrupt->Halt();
}


//------- MCGS 3---------------------------------------------------------
// CREATE REOMTE LOCK FUNCTION THIS IS CALLED FROM THE SERVER FUNCTION
// TO CREATE A LOCK 
//-----------------------------------------------------------------------
void createRemoteLock(char *name,int machineID)
{
	cout<<"\n The name of the requested lock creation is "<<name<<endl;
	cout<<"\n and its from machine with ID "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	char replyMsg[32];
	int lockFound=-1;
	int nameLen;
	nameLen=strlen(name);
	for(int i=0;i<MaxRpcLocksCondtions;i++) // CHECKING TO SEE IF THE LOCK IS ALREADY CREATED
	{
		if(strncmp(name,rpcLocks[i].name,nameLen)==0)
		{
			cout<<"\n Lock found"<<endl;
			lockFound=i;	// YES THE LOCK IS FOUND
		}
	}
	if(lockFound>=0)
	{
		// YES THE LOCK IS FOUND
		sprintf(replyMsg,"%d",lockFound);
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;

	    bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); // REPLYING BACK THE LOCK REFERNCE TO THE CLIENT

	   if ( !success )
		{
			 cout<<"\n NACHOS !!!! ERORR : SENDING FAILED !!!!"<<endl;
			 interrupt->Halt();
		}  
	   else
	     {
	       cout<<"\n The lock is already created and its reference is "<<lockFound<<endl;
	     }
	   return;

	}
	else // NO THE LOCK IS NOT YET CREATED
	{
		cout<<"\n The lock name is not found "<<endl;
		int lockRef=rpcLocksBM->Find(); // CREATING THE LOCK 
		strcpy(rpcLocks[lockRef].name,name);
		sprintf(replyMsg,"%d",lockRef);
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"\n NACHOS !!!! ERORR : SENDING FAILED !!!!"<<endl;
		  interrupt->Halt();
		}  
		else
		{
			cout<<"\n Reply message sent to client"<<endl;
		}
		cout<<"\n A lock is created in the server with with name: '"<<rpcLocks[lockRef].name<<"' and Id: "<<lockRef<<endl;
	}

	return;
}

//------- MCGS 3---------------------------------------------------------
// CREATE REOMTE LOCK FUNCTION THIS IS CALLED FROM THE SERVER FUNCTION
// TO ACQUIRE A LOCK 
//-----------------------------------------------------------------------

void AcquireRemoteLock(int lockRef,int machineID)
{
	//cout<<"\n Inside Remote Acquire lock function"<<endl;
	bool lockFound;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	
	lockFound=rpcLocksBM->Test(lockRef);
	if(lockFound) // CHECKING IF THE LOCK IS CREATED OR NOT
	{
		char replyMsg[32];
		if(rpcLocks[lockRef].owner==-1)
		{
			 rpcLocks[lockRef].owner=machineID;
			 sprintf(replyMsg,"%d",lockRef);
			 //---------------------------Sending data to the client
			 outPktHdr.to = machineID;		
			 outMailHdr.to = 1;
			 outMailHdr.from = 0;
			 outMailHdr.length = strlen(replyMsg) + 1;
			 bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 

			 if ( !success ) 
			 {
			   cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
			   interrupt->Halt();
			 }  
		}
		else
		{
		  rpcLocks[lockRef].waitQueue->Append((void*)machineID);
		  // IF THE LOCK IS NOT FREE NO REPLY MESSAGE IS SENT
		  // MAKING THE CLIENT TO WAIT ON THE RECEIVE FUCNTION
		  cout<<"\n!!!!! NACHOS SERVER : THE LOCK IS BUSY NO REPLY MSG SENT"<<endl;
		}
      
	}
	else // NO THE LOCK IS NOT YET CREATED
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested lock is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  
	
	}
}

//------- MCGS 3---------------------------------------------------------
// CREATE REOMTE LOCK FUNCTION THIS IS CALLED FROM THE SERVER FUNCTION
// TO RELEASE A LOCK 
//-----------------------------------------------------------------------
void ReleaseRemoteLock(int lockRef,int machineID)
{
	bool lockFound;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	//char replyMsg[32];
	lockFound=rpcLocksBM->Test(lockRef);
	if(lockFound) // CHECKING IF THE LOCK IS CREATED OR NOT
	{
		if(rpcLocks[lockRef].owner==machineID)
		{ // THE CLIENT IS THE OWNER OF THE LOCK WHICH WAS ALREADY CREATED
			
			if(rpcLocks[lockRef].waitQueue->IsEmpty())// CHECKING IF THE WAIT QUEUE IS EMPTY
			{
				// YES THE WAIT QUEUE IS EMPTY
				cout<<"\n NACHOS SERVER: The wait queue for the lock is empty so setting the lock owner to -1 "<<endl;
				rpcLocks[lockRef].owner=-1;		// SINCE WAIT QUEUE IS EMPTY SETTING THE OWNER TO -1
				char replyMsg[40];
				sprintf(replyMsg,"%d",lockRef);
				outPktHdr.to = machineID;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				   interrupt->Halt();
				}  
			}
			else // THE WAIT QUEUE IS NOT EMPTY
			{
				int nextOwner=(int)rpcLocks[lockRef].waitQueue->Remove();
				cout<<"\n !!!!! NACOS SEVER : The next client with machine id "<<nextOwner<<"is the owner of this lock "<<lockRef<<endl;
				rpcLocks[lockRef].owner=nextOwner;
				char replyMsg[40];
				sprintf(replyMsg,"%d",lockRef);
				outPktHdr.to = machineID;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				   interrupt->Halt();
				}
				
				outPktHdr.to = nextOwner;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				   interrupt->Halt();
				}  


			}
		}
		else // THE CLIENT IS NOT THE OWNER OF THE LOCK 
		{
			cout<<"!!!! NACHOS SERVER : Requested CLIENT is not the owner of the lock \n"<<endl;
			char replyMsg[40]="-1";
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
			   interrupt->Halt();
			}  
		}
      
	}
	else // NO THE LOCK IS NOT YET CREATED
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested lock is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  
	
	}
}

//------- MCGS 3---------------------------------------------------------
// CREATE REOMTE LOCK FUNCTION THIS IS CALLED FROM THE SERVER FUNCTION
// TO DESTROY A LOCK 
//-----------------------------------------------------------------------
void DestroyRemoteLock(int lockRef,int machineID)
{
	//cout<<"\n The name of the requested lock deletion is "<<name<<endl;
	//cout<<"\n and its from machine with ID "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	//char replyMsg[32];
	bool lockFound;
	lockFound=rpcLocksBM->Test(lockRef);
	if(lockFound)
	{
		// YES THE LOCK IS FOUND
		if(rpcLocks[lockRef].owner>=0) //  THE LOCK IS BUSY CANNOT DESTROY THE LOCK NOW
		{
			cout<<"\n!!!! NACHOS SERVER ERROR : Requested lock for destruction is busy cannot destory lock now\n"<<endl;
			char replyMsg[40]="-1";
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
			   interrupt->Halt();
			}  


		}
		else // THE LOCK IS FREE AND SO WE CAN GO AHEAD AND DELETE IT
		{
			rpcLocksBM->Clear(lockRef);
			rpcLocks[lockRef].owner=-1;
			rpcLocks[lockRef].name[0]='\0';
			char replyMsg[40];
			sprintf(replyMsg,"%d",lockRef);
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
			   interrupt->Halt();
			}  
		}


	}
	else // NO THE LOCK IS NOT YET CREATED
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested lock is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  
	
	}
	return;
}

//------- MCGS 3---------------------------------------------------------
// CREATE REOMTE LOCK FUNCTION THIS IS CALLED FROM THE SERVER FUNCTION
// TO CREATE A CONDITION 
//-----------------------------------------------------------------------

void CreateRemoteCondition(char *name,int machineID)
{
	cout<<"\n!!!! NACHOS SERVER : The name of the requested condition creation is "<<name<<endl;
	cout<<"\n!!!! NACHOS SERVER : and its from machine with ID "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	char replyMsg[32];
	int conditionFound=-1;
	int nameLen;
	nameLen=strlen(name);
	for(int i=0;i<MaxRpcLocksCondtions;i++) // CHECKING TO SEE IF THE CONDITION IS ALREADY CREATED
	{
		if(strncmp(name,rpcConditions[i].name,nameLen)==0)
		{
			cout<<"\n!!!! NACHOS SEVER : Condition found"<<endl;
			conditionFound=i;	// YES THE LOCK IS FOUND
		}
	}
	if(conditionFound>=0)
	{
		// YES THE LOCK IS FOUND
		sprintf(replyMsg,"%d",conditionFound);
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;

	    bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); // REPLYING BACK THE LOCK REFERNCE TO THE CLIENT

	   if ( !success )
		{
			 cout<<"\n !!!! NACHOS  ERORR : SENDING FAILED !!!!"<<endl;
			 interrupt->Halt();
		}  
	   else
	     {
	       cout<<"\n !!!! NACHOS SERVER : The condition is already created and its reference is "<<conditionFound<<endl;
	     }
	   return;

	}
	else // NO THE LOCK IS NOT YET CREATED
	{
		cout<<"\n !!!! NACHOS SERVER : The condition name is not found "<<endl;
		int conditionRef=rpcConditionsBM->Find(); // CREATING THE LOCK 
		strcpy(rpcConditions[conditionRef].name,name);
		sprintf(replyMsg,"%d",conditionRef);
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"\n!!!! NACHOS  ERORR : SENDING FAILED !!!!"<<endl;
		  interrupt->Halt();
		}  
		cout<<"\n!!!! NACHOS  ERORR : A condition is created in the server with with name: '"<<rpcConditions[conditionRef].name<<"' and Id: "<<conditionRef<<endl;
	}

	return;
}

void DestroyRemoteCondition(int conditionRef,int machineID)
{

	//cout<<"\n The name of the requested lock deletion is "<<name<<endl;
	//cout<<"\n and its from machine with ID "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	//char replyMsg[32];
	bool conditionFound;
	conditionFound=rpcConditionsBM->Test(conditionRef);
	if(conditionFound)
	{
		// YES THE CONDITION IS FOUND
		if(rpcConditions[conditionRef].waitQueue->IsEmpty()) //CHECKING IF CONDITION IS EMPTY
		{
			rpcConditionsBM->Clear(conditionRef);
			rpcConditions[conditionRef].lockId=-1;
			rpcConditions[conditionRef].name[0]='\0';
			char replyMsg[40];
			sprintf(replyMsg,"%d",conditionRef);
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
			   interrupt->Halt();
			}  
		}
		else // THE CONDTION QUEUE IS NOT EMPTY CANNOT DELETE THE CONDITION VARIABLE
		{
			cout<<"!!!! NACHOS SERVER ERROR : Requested condition's wait queue is not empty cannot destory the condition. !!!!"<<endl;
			char replyMsg[40]="-1";
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
			   interrupt->Halt();
			}  
		}
	}
	else // NO THE CONDITION IS NOT YET CREATED
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested condition is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  
	
	}
	return;

}

void WaitRemoteCondition(int conditionRef,int lockRef,int machineID)
{
	cout<<"\n The conditon number is "<<conditionRef<<endl;
	cout<<"\n The lock number is "<<lockRef<<endl;
	cout<<"\n The request is for condition wait from machine with id "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	bool lockFound;
	bool conditionFound;
	lockFound=rpcLocksBM->Test(lockRef);
	conditionFound=rpcConditionsBM->Test(conditionRef);

	if(lockFound && conditionFound) // CHECKING IF LOCK AND CONDTION VARIABLES ARE CREATED
	{
		// YES THEY ARE CREATED ALREADY
		if(rpcLocks[lockRef].owner==machineID) // CHECKING THE LOCK OWNERSHIP
		{
			if(rpcConditions[conditionRef].lockId==-1)
			{
				rpcConditions[conditionRef].lockId=lockRef;
			}
			if(rpcConditions[conditionRef].lockId==lockRef)
			{
					rpcConditions[conditionRef].waitQueue->Append((void *)machineID);
					cout<<"\n!!!! NACHOS SERVER : Machine Id added to condtions wait queue"<<endl;
					cout<<"\n !!!! NACHOS SERVER : Releaseing Lock"<<endl;
					if(rpcLocks[lockRef].waitQueue->IsEmpty())
					{
						cout<<"\n !!!! NACHOS SERVER : The Locks wait queue is empty"<<endl;
						rpcLocks[lockRef].owner=-1;
					}
					else
					{
						cout<<"\n The locks wait queue is not empty"<<endl;
						int nextOwner=(int)rpcLocks[lockRef].waitQueue->Remove();
						rpcLocks[lockRef].owner=nextOwner;
						char replyMsg[40];
						sprintf(replyMsg,"%d",lockRef);
						outPktHdr.to = nextOwner;		
						outMailHdr.to = 1;
						outMailHdr.from = 0;
						outMailHdr.length = strlen(replyMsg) + 1;
						bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
						if ( !success ) 
						{
						  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
						   interrupt->Halt();
						}  

					}
					//ReleaseRemoteLock(lockRef,machineID);
					cout<<"\n Release Lock called"<<endl;

			}
			else // THIS CONDITION IS NOT ON THIS LOCK 
			{
				cout<<"!!!! NACHOS SERVER ERROR : Requested condition is not on this lock \n"<<endl;
				char replyMsg[40]="-1";
				outPktHdr.to = machineID;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				   interrupt->Halt();
				}  
			}
		}
		else // THE CURRENT MACHINE IS NOT THE ONWER OF THE LOCK SO CANNOT WAIT ON THE LOCK 
		{
				cout<<"!!!! NACHOS SERVER ERROR : Requested machine is not the owner of this lock cannot cannot wait \n"<<endl;
				char replyMsg[40]="-1";
				outPktHdr.to = machineID;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
					interrupt->Halt();
				}  

		}
	}
	else // ETHER THE LOCK OR CONDITION VARIABLE IS NOT YET CREATED
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested condition or the lock  is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  


	}
}


void SignalRemoteCondition(int conditionRef,int lockRef,int machineID)
{
	//cout<<"\n The conditon number is "<<conditionRef<<endl;
	//cout<<"\n The lock number is "<<lockRef<<endl;
	//cout<<"\n The request is for condition signal from machine with id "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	bool lockFound;
	bool conditionFound;
	lockFound=rpcLocksBM->Test(lockRef);
	conditionFound=rpcConditionsBM->Test(conditionRef);
	if(lockFound && conditionFound) // CHECKING IF LOCK AND CONDTION VARIABLES ARE CREATED
	{
		// YES THEY ARE CREATED ALREADY
		if(rpcLocks[lockRef].owner==machineID) // CHECKING THE LOCK OWNERSHIP
		{
			if(rpcConditions[conditionRef].lockId==lockRef)
			{
				if(rpcConditions[conditionRef].waitQueue->IsEmpty())
				{
					cout<<"\n !!!! NACHOS SERVER : The condition wait queue is empty"<<endl;
					char replyMsg[40];
					sprintf(replyMsg,"%d",conditionRef);
					outPktHdr.to = machineID;		
					outMailHdr.to = 1;
					outMailHdr.from = 0;
					outMailHdr.length = strlen(replyMsg) + 1;
					bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
					if ( !success ) 
					{
					  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
					  interrupt->Halt();
					}  

				}
				else
				{
					cout<<"\n !!!! NACHOS SERVER :  The condition wait queue is not empty"<<endl;
					int tempMachineId=(int)rpcConditions[conditionRef].waitQueue->Remove();
					AcquireRemoteLock(lockRef,tempMachineId);
					//cout<<"\n Acquire Lock called"<<endl;
					char replyMsg[40];
					sprintf(replyMsg,"%d",conditionRef);
					outPktHdr.to = machineID;		
					outMailHdr.to = 1;
					outMailHdr.from = 0;
					outMailHdr.length = strlen(replyMsg) + 1;
					bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
					if ( !success ) 
					{
					  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
					  interrupt->Halt();
					}  

				}

			}
			else // THIS CONDITION IS NOT ON THIS LOCK 
			{
				cout<<"!!!! NACHOS SERVER ERROR : Requested condition is not on this lock \n"<<endl;
				char replyMsg[40]="-1";
				outPktHdr.to = machineID;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				   interrupt->Halt();
				}  
			}
		}
		else
		{
			cout<<"!!!! NACHOS SERVER ERROR : Requested machine is not the owner of this lock cannot call signal \n"<<endl;
			char replyMsg[40]="-1";
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				interrupt->Halt();
			}  
		}
	}
	else
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested condition or the lock  is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  
	}
}

void BroadcastRemoteCondition(int conditionRef,int lockRef,int machineID)
{
	//cout<<"\n The conditon number is "<<conditionRef<<endl;
	//cout<<"\n The lock number is "<<lockRef<<endl;
	//cout<<"\n The request is for condition Broadcast from machine with id "<<machineID<<endl;
	PacketHeader outPktHdr, inPktHdr;
	MailHeader outMailHdr, inMailHdr;
	bool lockFound;
	bool conditionFound;
	lockFound=rpcLocksBM->Test(lockRef);
	conditionFound=rpcConditionsBM->Test(conditionRef);
	if(lockFound && conditionFound) // CHECKING IF LOCK AND CONDTION VARIABLES ARE CREATED
	{
		// YES THEY ARE CREATED ALREADY
		if(rpcLocks[lockRef].owner==machineID) // CHECKING THE LOCK OWNERSHIP
		{
			if(rpcConditions[conditionRef].lockId==lockRef)
			{
				if(rpcConditions[conditionRef].waitQueue->IsEmpty())
				{
					cout<<"\n !!!! NACHOS SERVER : The condition wait queue is empty"<<endl;
					char replyMsg[40];
					sprintf(replyMsg,"%d",conditionRef);
					outPktHdr.to = machineID;		
					outMailHdr.to = 1;
					outMailHdr.from = 0;
					outMailHdr.length = strlen(replyMsg) + 1;
					bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
					if ( !success ) 
					{
					  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
					  interrupt->Halt();
					}  

				}
				else
				{
					cout<<"\n  !!!! NACHOS SERVER : The condition wait queue is not empty"<<endl;
					while(!rpcConditions[conditionRef].waitQueue->IsEmpty())
					{
						int tempMachineId=(int)rpcConditions[conditionRef].waitQueue->Remove();
						AcquireRemoteLock(lockRef,tempMachineId);
						//cout<<"\n Acquire Lock called"<<endl;
					}
					char replyMsg[40];
					sprintf(replyMsg,"%d",conditionRef);
					outPktHdr.to = machineID;		
					outMailHdr.to = 1;
					outMailHdr.from = 0;
					outMailHdr.length = strlen(replyMsg) + 1;
					bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
					if ( !success ) 
					{
					  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
					  interrupt->Halt();
					} 
				}

			}
			else // THIS CONDITION IS NOT ON THIS LOCK 
			{
				cout<<"!!!! NACHOS SERVER ERROR : Requested condition is not on this lock \n"<<endl;
				char replyMsg[40]="-1";
				outPktHdr.to = machineID;		
				outMailHdr.to = 1;
				outMailHdr.from = 0;
				outMailHdr.length = strlen(replyMsg) + 1;
				bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
				if ( !success ) 
				{
				  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				   interrupt->Halt();
				}  
			}
		}
		else
		{
			cout<<"!!!! NACHOS SERVER ERROR : Requested machine is not the owner of this lock cannot call signal \n"<<endl;
			char replyMsg[40]="-1";
			outPktHdr.to = machineID;		
			outMailHdr.to = 1;
			outMailHdr.from = 0;
			outMailHdr.length = strlen(replyMsg) + 1;
			bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
			if ( !success ) 
			{
			  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
				interrupt->Halt();
			}  
		}
	}
	else
	{
		cout<<"!!!! NACHOS SERVER ERROR : Requested condition or the lock  is not yet created\n"<<endl;
		char replyMsg[40]="-2";
		outPktHdr.to = machineID;		
		outMailHdr.to = 1;
		outMailHdr.from = 0;
		outMailHdr.length = strlen(replyMsg) + 1;
		bool success = postOffice->Send(outPktHdr, outMailHdr,replyMsg); 
		if ( !success ) 
		{
		  cout<<"!!!! NACHOS SERVER ERROR : SENDING FAILED\n"<<endl;
		   interrupt->Halt();
		}  
	}


}






void Server()
{
	cout<<"\n Nachos Server Started ......"<<endl;
	PacketHeader outPktHdr, inPktHdr;
    MailHeader outMailHdr, inMailHdr;
	char msgBuffer[512];
	char name[32];
	char *tempPtr;
	int requestID;
	while(1)
	{
		cout<<"\n Nachos server waiting to receive..."<<endl;
		postOffice->Receive(0,&inPktHdr, &inMailHdr, msgBuffer);
		printf("\n Got \"%s\" from %d, box %d\n",msgBuffer,inPktHdr.from,inMailHdr.from);
		//cout<<"\n Msg of 0 is" <<msgBuffer[0]<<" "<<msgBuffer[1]<<" "<<msgBuffer[3];
		sscanf(msgBuffer,"%d",&requestID);
		
		switch(requestID)
		{
			case 101: //  THE CLIENT HAS REQUESTED THE SERVICE OF CREATE A LOCK 
			{
				tempPtr=&msgBuffer[3];
				strcpy(name,tempPtr);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to create a lock. !!!!"<<endl;
				createRemoteLock(name,inPktHdr.from);  
			}
			break;
			case 102: //  THE CLIENT HAS REQUESTED THE SERVICE OF ACQUIRE A LOCK 
			{
				tempPtr=&msgBuffer[4];
				strcpy(name,tempPtr);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to Acquire a lock. !!!!"<<endl;
				int temp;
				sscanf(name,"%d",&temp);
				AcquireRemoteLock(temp,inPktHdr.from);  
			}
			break;
			case 103: //  THE CLIENT HAS REQUESTED THE SERVICE OF RELEASE A  LOCK 
			{
				tempPtr=&msgBuffer[4];
				strcpy(name,tempPtr);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to Release a lock. !!!!"<<endl;
				int temp;
				sscanf(name,"%d",&temp);
				ReleaseRemoteLock(temp,inPktHdr.from);  
			}
			break;
			case 104: //  THE CLIENT HAS REQUESTED THE SERVICE OF DESTROY A LOCK 
			{
				tempPtr=&msgBuffer[4];
				strcpy(name,tempPtr);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to Destroy a lock. !!!!"<<endl;
				int temp;
				sscanf(name,"%d",&temp);
				DestroyRemoteLock(temp,inPktHdr.from);  
			}
			break;
			case 105: //  THE CLIENT HAS REQUESTED THE SERVICE OF CREATE A CONDITION 	
			{
				tempPtr=&msgBuffer[3];
				strcpy(name,tempPtr);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to create a condition. !!!!"<<endl;
				CreateRemoteCondition(name,inPktHdr.from);  
			}
			break;
			case 106: //  THE CLIENT HAS REQUESTED THE SERVICE OF DESTROY A CONDITION 	
			{
				tempPtr=&msgBuffer[4];
				strcpy(name,tempPtr);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to Destroy a condition. !!!!"<<endl;
				int temp;
				sscanf(name,"%d",&temp);
				DestroyRemoteCondition(temp,inPktHdr.from);  

			}
			break;
			case 107: //  THE CLIENT HAS REQUESTED THE SERVICE OF WAIT ON A CONDITION 	
			{
				char condition[32];
				char lock[32];
				tempPtr=&msgBuffer[4];
				char *tempPtr2=&msgBuffer[6];
				strcpy(condition,tempPtr);
				strcpy(lock,tempPtr2);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to wait on a condition. !!!!"<<endl;
				int conditionNum;
				int lockNum;
				sscanf(condition,"%d",&conditionNum);
				sscanf(lock,"%d",&lockNum);
				WaitRemoteCondition(conditionNum,lockNum,inPktHdr.from);
			}
			break;
			case 108:
			{
								char condition[32];
				char lock[32];
				tempPtr=&msgBuffer[4];
				char *tempPtr2=&msgBuffer[6];
				strcpy(condition,tempPtr);
				strcpy(lock,tempPtr2);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to Destroy a condition. !!!!"<<endl;
				int conditionNum;
				int lockNum;
				sscanf(condition,"%d",&conditionNum);
				sscanf(lock,"%d",&lockNum);
				SignalRemoteCondition(conditionNum,lockNum,inPktHdr.from);

			}
			break;
			case 109:
			{
				char condition[32];
				char lock[32];
				tempPtr=&msgBuffer[4];
				char *tempPtr2=&msgBuffer[6];
				strcpy(condition,tempPtr);
				strcpy(lock,tempPtr2);
				cout<<"\n !!!! Nachos Server : The cliet  has requested the service to Destroy a condition. !!!!"<<endl;
				int conditionNum;
				int lockNum;
				sscanf(condition,"%d",&conditionNum);
				sscanf(lock,"%d",&lockNum);
				BroadcastRemoteCondition(conditionNum,lockNum,inPktHdr.from);

			}
			break;



		}
	}
	cout<<"\n"<<endl;
}