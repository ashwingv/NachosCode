#include "syscall.h"

#define totalNumberOfValets 10		
#define totalNumberOfCars 2			
#define totalNumberOfDrivers 2		
#define totalCountOfLimos 2			
#define totalCountOfLimoDrivers 2	
#define totalNumberOfVehicles 4
#define grandTotalOfVisitors 4
#define totalNumberOfTicketTakers 2

int CV_waitingForValetManager[totalNumberOfValets];

int CV_waitingInBackRoom[totalNumberOfValets];

int valetManagerWaiting=0;
int valetManagerWaitingLock=-1;
int CV_valetManagerWaiting=-1;


int valetStatus[totalNumberOfValets];				
													
													
													
int valetStatusLock[totalNumberOfValets];			
int CV_valetWaitingForDriver[totalNumberOfValets];	

int countOfWaitingValets=0;							
													
int waitingValetCountLock=-1;						
int CV_countOfWaitingValets=-1;						


int visitor[grandTotalOfVisitors];					


int limoQueueLength=0;								
int limoQueueLengthLock=-1;							
int CV_limosWaitingToGetParked=-1;					

int carQueueLength=0;								
											
int carQueueLengthLock=-1;							
int CV_waitingToGetParked=-1;						

int exitLimoQueueLength=0;
int exitLimoQueueLengthLock=-1;
int CV_exitLimoQueueLength=-1;

int exitCarQueueLength=0;
int exitCarQueueLengthLock=-1;
int CV_exitCarQueueLength=-1;

int readyToPark[totalNumberOfVehicles];				
int readyToParkLock[totalNumberOfVehicles];			
int CV_waitingToParkCar[totalNumberOfVehicles];		

int visitorPleaseGetOut[totalNumberOfVehicles];		
int visitorPleaseGetOutLock[totalNumberOfVehicles]; 
int CV_visitorPleaseGetOut[totalNumberOfVehicles];	

int visitorPleaseGetIn[totalNumberOfVehicles];
int visitorPleaseGetInLock[totalNumberOfVehicles];
int CV_visitorPleaseGetIn[totalNumberOfVehicles];

int totalNumberOfVisitors[totalNumberOfVehicles];	
int totalNumberOfVisitorsLock[totalNumberOfVehicles];

int visitorCount[totalNumberOfVehicles];			
int visitorCountLock[totalNumberOfVehicles];		
int CV_visitorCount[totalNumberOfVehicles];			

int carParked[totalNumberOfVehicles];				
int carParkedLock[totalNumberOfVehicles];			

int parkingToken[totalNumberOfVehicles];			
int parkingTokenLock[totalNumberOfVehicles];		
int CV_parkingToken[totalNumberOfVehicles];			

int valetReadyWithCar[totalNumberOfVehicles];		 
int valetReadyWithCarLock[totalNumberOfVehicles];	 
int CV_valetReadyWithCar[totalNumberOfVehicles];	 

int visitorsReadyToGoHome[totalNumberOfVehicles];  	 
													
int visitorsReadyToGoHomeLock[totalNumberOfVehicles];
int CV_visitorReadyToGoHome[totalNumberOfVehicles];	 

int driverStatus[totalNumberOfVehicles];			 
int driverStatusLock[totalNumberOfVehicles];		
int CV_driverStatus[totalNumberOfVehicles];			



int tipsForThisCar[totalNumberOfVehicles];			
int tipsForThisCarLock[totalNumberOfVehicles];		
int CV_tipsForThisCar[totalNumberOfVehicles];		

int carKeysReady[totalNumberOfVehicles];			
int carKeysReadyLock[totalNumberOfVehicles];		
int CV_carKeysReady[totalNumberOfVehicles];			

int carReadyToGo[totalNumberOfVehicles];			
int carReadyToGoLock[totalNumberOfVehicles];		
int CV_carReadyToGo[totalNumberOfVehicles];

				
int totalNumberOfActiveTicketTakers=0;				
int totalNumberOfActiveTicketTakersLock;			
int CV_noTicketTakers;							



int ticketTakerLock=-1;
int CV_ticketTaker=-1;

int ticketTakerQueueLength[totalNumberOfTicketTakers];		
int ticketTakerQueueLengthLock[totalNumberOfTicketTakers];  
int CV_ticketTakerQueueLength[totalNumberOfTicketTakers];	
int CV_waitingTicketTaker[totalNumberOfTicketTakers];		

int totalNumberOfVisitorsAccepted=0;					
int totalNumberOfVisitorsAcceptedLock;			
int visitorMuseumEntryStatus[grandTotalOfVisitors];															
int visitorMuseumEntryStatusLock[grandTotalOfVisitors];	
int CV_visitorMuseumEntryStatus[grandTotalOfVisitors];  

int valetID=0;
int valetIDLock=-1;

int limoDriverID=totalNumberOfDrivers;
int limoDriverIDLock=-1;

int driverID=0;
int driverIDLock=-1;

int visitorID=0;
int visitorIDLock=-1;

int ticketTakerID=0;
int ticketTakerIDLock=-1;

/*-------------------------------------------------------------*/
int myexp ( int count ) {
  int i, val=1;
  for (i=0; i<count; i++ ) {
    val = val * 10;
  }
  return val;
}

void itoa( char arr[], int size, int val ) {
  int i, max, dig, subval, loc;
  for (i=0; i<size; i++ ) {
    arr[i] = '\0';
  }

  for ( i=1; i<=2; i++ ) {
    if (( val / myexp(i) ) == 0 ) {
      max = i-1;
      break;
    }
  }

  subval = 0;
  loc = 0;
  for ( i=max; i>=0; i-- ) {
    dig = 48 + ((val-subval) / myexp(i));
    subval = (dig-48) * myexp(max);
    arr[loc] = dig;
    loc++;
  }

  return;
}

/*--------------------------------------------------------------------------------*/

void valet()
{
   
	int id=-1;
    int carToService=-1; 
	int token=-1;

	AcquireLock(valetIDLock);
	id=valetID;
	valetID++;
	ReleaseLock(valetIDLock);
	while(1)
	{
					/*valetStatusLock[id]->Acquire(); // ADDED TO AID INTERACTION FOR RETURNING CARS */
					AcquireLock(valetStatusLock[id]);
					
					valetStatus[id]=-5;				/*// ADDED TO AID INTERACTION FOR RETURNING CARS*/

					/*valetStatusLock[id]->Release();	// ADDED TO AID INTERACTION FOR RETURNING CARS*/
					ReleaseLock(valetStatusLock[id]);

					/*waitingValetCountLock->Acquire();*/
					AcquireLock(waitingValetCountLock);

					countOfWaitingValets++;

					/*printf("\n ++ The count of waiting valet in bench is %d \n",countOfWaitingValets);*/
					Write("\n ++ Valet is waiting int bench",31,1);

					/*CV_countOfWaitingValets->Wait(waitingValetCountLock);*/
					WaitCondition(CV_countOfWaitingValets,waitingValetCountLock);

					countOfWaitingValets--;

					/*printf("\n -- The count of waiting valet in bench is %d \n",countOfWaitingValets);*/
					Write("\n -- Valet is up from the bench",31,1);

					/*waitingValetCountLock->Release();*/
					ReleaseLock(waitingValetCountLock);

					/*valetManagerWaitingLock->Acquire();*/
					AcquireLock(valetManagerWaitingLock);

					if(valetManagerWaiting==1)
					{
						/*printf("\n The valet %d is reporting to Valet Manager",id);*/
						Write("\n Valet is reporting to manager",31,1);

						/*valetStatusLock[id]->Acquire();*/
						AcquireLock(valetStatusLock[id]);

						valetStatus[id]=-9;
						valetManagerWaiting--;

						/*CV_valetManagerWaiting->Signal(valetManagerWaitingLock);*/
						SignalCondition(CV_valetManagerWaiting,valetManagerWaitingLock);

						/*valetManagerWaitingLock->Release();*/
						ReleaseLock(valetManagerWaitingLock);

						/*CV_waitingForValetManager[id]->Wait(valetStatusLock[id]);*/
						WaitCondition(CV_waitingForValetManager[id],valetStatusLock[id]);

						if(valetStatus[id]==-10)
						{
							/*printf("\n Valet %d is in the back room ", id);*/
							Write("\n Valet is in back room",23,1);

							/*CV_waitingInBackRoom[id]->Wait(valetStatusLock[id]);*/
							WaitCondition(CV_waitingInBackRoom[id],valetStatusLock[id]);

							/*printf("\n Valet %d is out of the back room ", id);*/
							Write("\n Valet is out of the back room",31,1);
						}
						/*valetStatusLock[id]->Release();*/
						ReleaseLock(valetStatusLock[id]);
						
					}
					else
					{
							/*valetManagerWaitingLock->Release();*/
							ReleaseLock(valetManagerWaitingLock);

					}

		
			/*--------------------------------------SERVICING LIMOS PARKING FIRST---------------------------------------------*/
	        /*limoQueueLengthLock->Acquire();*/
			AcquireLock(limoQueueLengthLock);

			while(limoQueueLength>0)
            {
                limoQueueLength--;

                /*valetStatusLock[id]->Acquire();*/
				AcquireLock(valetStatusLock[id]);
                valetStatus[id]=-1; /* SETS HIS STATUS TO SERVICE A LIMO */

                /*CV_limosWaitingToGetParked->Signal(limoQueueLengthLock); // SIGNALS A CAR*/
				SignalCondition(CV_limosWaitingToGetParked,limoQueueLengthLock);

                /*limoQueueLengthLock->Release();*/
				ReleaseLock(limoQueueLengthLock);

                /*CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR*/
				WaitCondition(CV_valetWaitingForDriver[id],valetStatusLock[id]);

                carToService=valetStatus[id]; /* WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE*/

                /*printf("\n Valet %d is busy with Limousine %d",id,carToService);*/
				Write("\n Valet busy with Limosine",26,1);

                /*valetStatusLock[id]->Release();*/
				ReleaseLock(valetStatusLock[id]);

                /*valetReadyWithCarLock[carToService]->Acquire();  // NOW THE VALET IS READY TO SERVICE A PARTICULAR CAR*/
				AcquireLock(valetReadyWithCarLock[carToService]);

                valetReadyWithCar[carToService]=1;

                /*CV_valetReadyWithCar[carToService]->Signal(valetReadyWithCarLock[carToService]); // SO HE SIGNALS THE DRIVER THAT HE IS WITH HIS CAR*/
				SignalCondition(CV_valetReadyWithCar[carToService],valetReadyWithCarLock[carToService]);

                /*valetReadyWithCarLock[carToService]->Release();*/
				ReleaseLock(valetReadyWithCarLock[carToService]);

                /*readyToParkLock[carToService]->Acquire();*/
				AcquireLock(readyToParkLock[carToService]);

                while(readyToPark[carToService]!=1) 
                {
                    /*/CAR IS NOT READY TO PARK SO HE WAITS FOR IT TO BECOME READY*/
                   /* printf("\n Valet %d is waiting to park Limousine %d",id,carToService);*/
				   Write("\n Valet is waiting to park limosuine",36,1);

				   /*CV_waitingToParkCar[carToService]->Wait(readyToParkLock[carToService]);*/
				   WaitCondition(CV_waitingToParkCar[carToService],readyToParkLock[carToService]);

                }
                /* THE CAR IS NOW READY TO PARK*/
                /*readyToParkLock[carToService]->Release();*/
				ReleaseLock(readyToParkLock[carToService]);

                /*printf("\n Valet %d got the keys for the Limousine %d",id,carToService); // GETS THE KEYS*/
				Write("\n Valet got the keys to the limo",32,1);

				/*parkingTokenLock[carToService]->Acquire();*/
				AcquireLock(parkingTokenLock[carToService]);

				parkingToken[carToService]=carToService;

				/*CV_parkingToken[carToService]->Signal(parkingTokenLock[carToService]);*/
				SignalCondition(CV_parkingToken[carToService],parkingTokenLock[carToService]);

				/*printf("\n Valet %d is giving the parking token to Limosuine driver %d",id,carToService);*/
				Write("\n Valet giving parking token to limo driver",43,1);

				/*parkingTokenLock[carToService]->Release();*/
				ReleaseLock(parkingTokenLock[carToService]);

                /*printf("\n Valet %d is parking Limousine %d",id,carToService); // PARKS THE CAR*/
				Write("\n Valet is parking Limo",24,1);
					/*currentThread->Yield();*/
					Yield();
					/*currentThread->Yield();*/

					/*currentThread->Yield();*/
					Yield();
					
					/*currentThread->Yield();*/
					Yield();

					/*currentThread->Yield();*/
					Yield();

					/*currentThread->Yield();*/
					Yield();

				/*carParkedLock[carToService]->Acquire();*/
				AcquireLock(carParkedLock[carToService]);

				carParked[carToService]=carToService;

				/*carParkedLock[carToService]->Release();*/
				ReleaseLock(carParkedLock[carToService]);

				/*limoQueueLengthLock->Acquire();*/
				AcquireLock(limoQueueLengthLock);

            }
			/*limoQueueLengthLock->Release();*/
			ReleaseLock(limoQueueLengthLock);

			/*--------------------------------------SERVICING CARS PARKING FIRST---------------------------------------------*/
			/*carQueueLengthLock->Acquire();*/
			AcquireLock(carQueueLengthLock);

			while(carQueueLength>0)
			{
				carQueueLength--; /* DECREMENTS THE CAR QUEUE LENGTH SINCE HE IS GOIN TO SIGNAL A CAR               */
                /*valetStatusLock[id]->Acquire();*/
				AcquireLock(valetStatusLock[id]);

                valetStatus[id]=-2; /* SETS HIS STATUS TO BE FREE*/

                /*CV_waitingToGetParked->Signal(carQueueLengthLock); // SIGNALS A CAR*/
				SignalCondition(CV_waitingToGetParked,carQueueLengthLock);

                /*carQueueLengthLock->Release();*/
				ReleaseLock(carQueueLengthLock);

                /*CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR*/
				WaitCondition(CV_valetWaitingForDriver[id],valetStatusLock[id]);

                carToService=valetStatus[id]; /* WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE */
                /*printf("\n Valet %d is busy with car %d",id,carToService);*/
				Write("\n Valet busy with a car",23,1);

                /*valetStatusLock[id]->Release();*/
				ReleaseLock(valetStatusLock[id]);

                /*valetReadyWithCarLock[carToService]->Acquire();  // NOW THE VALET IS READY TO SERVICE A PARTICULAR CAR*/
				AcquireLock(valetReadyWithCarLock[carToService]);

                valetReadyWithCar[carToService]=1;

                /*CV_valetReadyWithCar[carToService]->Signal(valetReadyWithCarLock[carToService]); // SO HE SIGNALS THE DRIVER THAT HE IS WITH HIS CAR*/
				SignalCondition(CV_valetReadyWithCar[carToService],valetReadyWithCarLock[carToService]);

                /*valetReadyWithCarLock[carToService]->Release();*/
				ReleaseLock(valetReadyWithCarLock[carToService]);

                /*readyToParkLock[carToService]->Acquire();*/
				ReleaseLock(readyToParkLock[carToService]);

				/*carParkedLock[carToService]->Acquire();*/
				AcquireLock(carParkedLock[carToService]);

                while(readyToPark[carToService]!=1) /* THEN CHECKS IF THE CAR IS READY TO PARK */
                {
                        /*CAR IS NOT READY TO PARK SO HE WAITS FOR IT TO BECOME READY*/
                        /*printf("\n Valet %d is waiting to park car %d",id,carToService);*/
						Write("\n Valet is waitng to park a car",31,1);

                        /*CV_waitingToParkCar[carToService]->Wait(readyToParkLock[carToService]);*/
						WaitCondition(CV_waitingToParkCar[carToService],readyToParkLock[carToService]);
                }
                /* THE CAR IS NOW READY TO PARK*/
                /*readyToParkLock[carToService]->Release();*/
				ReleaseLock(readyToParkLock[carToService]);

                /*printf("\n Valet %d got the keys for the car %d",id,carToService); // GETS THE KEYS*/
				Write("\n Valet got the keys to the car",31,1);

				/*parkingTokenLock[carToService]->Acquire();*/
				AcquireLock(parkingTokenLock[carToService]);

				parkingToken[carToService]=carToService;

				/*CV_parkingToken[carToService]->Signal(parkingTokenLock[carToService]);*/
				SignalCondition(CV_parkingToken[carToService],parkingTokenLock[carToService]);

				/*printf("\n Valet %d is giving the parking token to driver %d",id,carToService);*/
				Write("\n valet is giving parking token to driver",41,1);
				/*parkingTokenLock[carToService]->Release();*/
				ReleaseLock(parkingTokenLock[carToService]);

                /*printf("\n Valet %d is parking car %d",id,carToService); // PARKS THE CAR*/
				Write("\n Valet parking car ",20,1);
					/*currentThread->Yield();*/
					Yield();
					/*currentThread->Yield();*/
					Yield();
					/*currentThread->Yield();*/
					Yield();
					/*currentThread->Yield();*/
					Yield();
					/*currentThread->Yield();*/
					Yield();
					/*currentThread->Yield();*/
					Yield();

				/*carParkedLock[carToService]->Acquire();*/
				AcquireLock(carParkedLock[carToService]);

				carParked[carToService]=carToService;

				/*carParkedLock[carToService]->Release();*/
				ReleaseLock(carParkedLock[carToService]);

				/*carQueueLengthLock->Acquire();*/
				AcquireLock(carQueueLengthLock);
			}
			/*carQueueLengthLock->Release();*/
			ReleaseLock(carQueueLengthLock);

			/*--------------------------------------SERVICING EXIT LIMOS THIRD -------------------------------------------*/
			
			/*exitLimoQueueLengthLock->Acquire();*/
			AcquireLock(exitLimoQueueLengthLock);

			while(exitLimoQueueLength>0)
			{
				exitLimoQueueLength--;

                /*valetStatusLock[id]->Acquire();*/
				AcquireLock(valetStatusLock[id]);

                valetStatus[id]=-3; /* SETS HIS STATUS TO BE FREE*/

				/*CV_exitLimoQueueLength->Signal(exitLimoQueueLengthLock);*/
				SignalCondition(CV_exitLimoQueueLength,exitLimoQueueLengthLock);

				/*exitLimoQueueLengthLock->Release();*/
				ReleaseLock(exitLimoQueueLengthLock);

                /*CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR*/
				WaitCondition(CV_valetWaitingForDriver[id],valetStatusLock[id]);

                carToService=valetStatus[id]; /* WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE*/

                /*printf("\n Valet %d is busy with Limosuine %d",id,carToService);*/
				Write("\n Valet is busy with exit limousine",35,1);

                /*valetStatusLock[id]->Release();*/
				ReleaseLock(valetStatusLock[id]);

				/*carParkedLock[carToService]->Acquire();*/
				AcquireLock(carParkedLock[carToService]);

				if(carParked[carToService]==carToService)
				{
					/*printf("\n Valet %d got the token from Limousine driver %d,",id,carToService);*/
					Write("\n Valet got the parking token from limo driver",46,1);

					/*carParkedLock[carToService]->Release();*/
					ReleaseLock(carParkedLock[carToService]);

					/*printf("\n Valet %d is bringing back the Limousine %d",id,carToService);*/
					Write("\n Valet bringing back Limo",27,1);

					/*rrentThread->Yield();*
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();*/
					Yield();
					Yield();
					Yield();
					Yield();
					Yield();
					Yield();

					/*carReadyToGoLock[carToService]->Acquire();*/
					AcquireLock(carReadyToGoLock[carToService]);

					carReadyToGo[carToService]=1;

					/*CV_carReadyToGo[carToService]->Signal(carReadyToGoLock[carToService]);*/
					SignalCondition(CV_carReadyToGo[carToService],carReadyToGoLock[carToService]);

					/*carReadyToGoLock[carToService]->Release();*/
					ReleaseLock(carReadyToGoLock[carToService]);
				}

				/*tipsForThisCarLock[carToService]->Acquire();*/
				AcquireLock(tipsForThisCarLock[carToService]);

				while(tipsForThisCar[carToService]<=0)
				{
					/*printf("\n Valet %d is waiting to get tips from the Limousine driver %d",id,carToService);*/
					Write("\n Valet is waiting to get tip from limo driver",47,1);

					/*CV_tipsForThisCar[carToService]->Wait(tipsForThisCarLock[carToService]);*/
					WaitCondition(CV_tipsForThisCar[carToService],tipsForThisCarLock[carToService]);
				}
				/*printf("\n Valet %d has got the tip from Limousine driver %d of amount $ %d \n ",id,carToService,tipsForThisCar[carToService]);*/
				Write("\n Valet got the tip from limo driver",35,1);

				/*tipsForThisCarLock[carToService]->Release();*/
				ReleaseLock(tipsForThisCarLock[carToService]);

				/*carKeysReadyLock[carToService]->Acquire();*/
				AcquireLock(carKeysReadyLock[carToService]);

				carKeysReady[carToService]=1;

				/*printf("\n Valet %d is giving the key to Limousine %d",id,carToService);*/
				Write("\n Valet giving the key to limo",31,1);

				/*CV_carKeysReady[carToService]->Signal(carKeysReadyLock[carToService]);*/
				SignalCondition(CV_carKeysReady[carToService],carKeysReadyLock[carToService]);

				/*carKeysReadyLock[carToService]->Release();*/
				ReleaseLock(carKeysReadyLock[carToService]);

				/*countOfExitedCarLock->Acquire();*/
				/*AcquireLock(countOfExitedCarLock);*/

				/*countOfExitedCars++;*/

				/*countOfExitedCarLock->Release();*/
				/*ReleaseLock(countOfExitedCarLock);*/

				/*exitCarQueueLengthLock->Acquire();*/
				AcquireLock(exitCarQueueLengthLock);
			}
			/*exitLimoQueueLengthLock->Release();*/
			ReleaseLock(exitLimoQueueLengthLock);

			/*--------------------------------------SERVICING EXIT CARS FOURTH--------------------------------------------*/
			/*exitCarQueueLengthLock->Acquire();*/
			AcquireLock(exitCarQueueLengthLock);

			while(exitCarQueueLength>0)
			{
				exitCarQueueLength--;
                /*valetStatusLock[id]->Acquire();*/
				AcquireLock(valetStatusLock[id]);
                valetStatus[id]=-4; /* SETS HIS STATUS TO BE FREE*/

				/*CV_exitCarQueueLength->Signal(exitCarQueueLengthLock);*/
				SignalCondition(CV_exitCarQueueLength,exitCarQueueLengthLock);

				/*exitCarQueueLengthLock->Release();*/
				ReleaseLock(exitCarQueueLengthLock);

                /*CV_valetWaitingForDriver[id]->Wait(valetStatusLock[id]); // WAITS TO GET ASSOCIATED WITH A CAR*/
				WaitCondition(CV_valetWaitingForDriver[id],valetStatusLock[id]);

                carToService=valetStatus[id]; /* WAKES UP ON A SIGNAL AND RECORDS THE CAR NUMBER HE IS GOIN TO SERVICE*/

                /*printf("\n Valet %d is busy with car %d",id,carToService);*/
				Write("\n Valet busy with exit car",26,1);

                /*valetStatusLock[id]->Release();*/
				ReleaseLock(valetStatusLock[id]);

				/*carParkedLock[carToService]->Acquire();*/
				AcquireLock(carParkedLock[carToService]);

				if(carParked[carToService]==carToService)
				{
					/*carParkedLock[carToService]->Release();*/
					ReleaseLock(carParkedLock[carToService]);

					/*printf("\n Valet %d is bringing back the car %d",id,carToService);*/
					Write("\n Valet bring back car",22,1);

					/*currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();
					currentThread->Yield();*/
					Yield();
					Yield();
					Yield();
					Yield();
					Yield();
					Yield();

					/*carReadyToGoLock[carToService]->Acquire();*/
					AcquireLock(carReadyToGoLock[carToService]);

					carReadyToGo[carToService]=1;

					/*CV_carReadyToGo[carToService]->Signal(carReadyToGoLock[carToService]);*/
					SignalCondition(CV_carReadyToGo[carToService],carReadyToGoLock[carToService]);

					/*carReadyToGoLock[carToService]->Release();*/
					ReleaseLock(carReadyToGoLock[carToService]);
				}
				/*tipsForThisCarLock[carToService]->Acquire();*/
				AcquireLock(tipsForThisCarLock[carToService]);

				while(tipsForThisCar[carToService]<=0)
				{
					/*printf("\nValet %d is waiting to get tips from the driver %d",id,carToService);*/
					Write("\n valet is waiting to get tip",29,1);

					/*CV_tipsForThisCar[carToService]->Wait(tipsForThisCarLock[carToService]);*/
					WaitCondition(CV_tipsForThisCar[carToService],tipsForThisCarLock[carToService]);
				}
				/*printf("\n Valet %d has got the tip from driver %d of amount $ %d \n ",id,carToService,tipsForThisCar[carToService]);*/
				Write("\n Valet got tip",15,1);

				/*tipsForThisCarLock[carToService]->Release();*/
				ReleaseLock(tipsForThisCarLock[carToService]);

				/*carKeysReadyLock[carToService]->Acquire();*/
				AcquireLock(carKeysReadyLock[carToService]);

				carKeysReady[carToService]=1;

				/*printf("\n Valet %d is giving the key to driver %d",id,carToService);*/
				Write("\n valet giving keys",19,1);

				/*CV_carKeysReady[carToService]->Signal(carKeysReadyLock[carToService]);*/
				SignalCondition(CV_carKeysReady[carToService],carKeysReadyLock[carToService]);

				/*carKeysReadyLock[carToService]->Release();*/
				ReleaseLock(carKeysReadyLock[carToService]);

				/*countOfExitedCarLock->Acquire();*/
				/*AcquireLock(countOfExitedCarLock);
				countOfExitedCars++;*/
				/*countOfExitedCarLock->Release();*/
				/*ReleaseLock(countOfExitedCarLock);*/

				/*exitCarQueueLengthLock->Acquire();*/
				AcquireLock(exitCarQueueLengthLock);
			}
			/*exitCarQueueLengthLock->Release();*/
			ReleaseLock(exitCarQueueLengthLock);
	}
	Exit(0);
}

void ticketTakerFunction()
{
    int id; /* RECORDS HIS IDENTITY */
	AcquireLock(ticketTakerIDLock);
	id=ticketTakerID;
	ticketTakerID++;
	ReleaseLock(ticketTakerIDLock);

    /*totalNumberOfActiveTicketTakersLock->Acquire();*/
	AcquireLock(totalNumberOfActiveTicketTakersLock);

	totalNumberOfActiveTicketTakers++;

    /*CV_noTicketTakers->Broadcast(totalNumberOfActiveTicketTakersLock);*/
	BroadcastCondition(CV_noTicketTakers,totalNumberOfActiveTicketTakersLock);

	/*totalNumberOfActiveTicketTakersLock->Release();*/
	ReleaseLock(totalNumberOfActiveTicketTakersLock);
	while(1)
	{
		/*ticketTakerQueueLengthLock[id]->Acquire();*/
		AcquireLock(ticketTakerQueueLengthLock[id]);
		if(ticketTakerQueueLength[id]==0)
		{
			/*printf("\n Ticket Taker %d finds no visitor in his queue so he waits ",id);*/
			Write("\nTicket taker finds no visitors so he waits",43,1);
			/*visitorMuseumEntryStatusLock[id]->Acquire();*/
			AcquireLock(visitorMuseumEntryStatusLock[id]);

			visitorMuseumEntryStatus[id]=0;

			/*visitorMuseumEntryStatusLock[id]->Release();*/
			ReleaseLock(visitorMuseumEntryStatusLock[id]);

			/*CV_waitingTicketTaker[id]->Wait(ticketTakerQueueLengthLock[id]);*/
			WaitCondition(CV_waitingTicketTaker[id],ticketTakerQueueLengthLock[id]);
		}
		Write("\nTicket awake",13,1);

		while(ticketTakerQueueLength[id]>0)
		{
			/*visitorMuseumEntryStatusLock[id]->Acquire();*/
			AcquireLock(visitorMuseumEntryStatusLock[id]);
			visitorMuseumEntryStatus[id]=1;
			/*CV_visitorMuseumEntryStatus[id]->Signal(visitorMuseumEntryStatusLock[id]);*/
			SignalCondition(CV_visitorMuseumEntryStatus[id],visitorMuseumEntryStatusLock[id]);
			ticketTakerQueueLength[id]--;
			/*printf("\n Ticket taker %d accepting the ticket from a visitor",id); */
			Write("\nTicket taker is accepting a ticket from the visitor",52,1);
			/*visitorMuseumEntryStatusLock[id]->Release();*/
			ReleaseLock(visitorMuseumEntryStatusLock[id]);
		}
		/*ticketTakerQueueLengthLock[id]->Release();*/
		ReleaseLock(ticketTakerQueueLengthLock[id]);

	}
	Exit(0);
    
}


void driver()
{
	char buf[2];
    int id=-1;
	int myParkingToken=-1;
	int myTicketTaker=-1;
	int leastLength;
	int j=0;
	AcquireLock(driverIDLock);
	id=driverID;
	driverID++;
	ReleaseLock(driverIDLock);
    
	/*carQueueLengthLock->Acquire();*/
	AcquireLock(carQueueLengthLock);

    carQueueLength++; /* HE INCREMENTS THE CAR QUEUE LENGTH */
    /*printf("\n Number of waiting cars is %d",carQueueLength);*/
    /*waitingValetCountLock->Acquire();*/
	AcquireLock(waitingValetCountLock);

    if(countOfWaitingValets>0) /* CHECKS IF ANY VALET IS IN BENCH */
    {
        /* THERE IS ONE FOUND IN THE BENCH SO THE DRIVER SIGNALS HIM*/
        /*CV_countOfWaitingValets->Signal(waitingValetCountLock);*/
		SignalCondition(CV_countOfWaitingValets,waitingValetCountLock);
    }

    /*waitingValetCountLock->Release();*/
	ReleaseLock(waitingValetCountLock);

    /*CV_waitingToGetParked->Wait(carQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED*/
	WaitCondition(CV_waitingToGetParked,carQueueLengthLock);

    /*carQueueLengthLock->Release();*/
	ReleaseLock(carQueueLengthLock);

    /* ITS THE DRIVER CHANCE NOW */
    for(j=0;j<totalNumberOfValets;j++)
    {
        /* SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE */

        /*valetStatusLock[j]->Acquire();*/
		AcquireLock(valetStatusLock[j]);

        if(valetStatus[j]==-2)
        {
            
            valetStatus[j]=id;
            /* CAR DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM */
            /*CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);*/
			SignalCondition(CV_valetWaitingForDriver[j],valetStatusLock[j]);

            /*valetStatusLock[j]->Release();*/
			ReleaseLock(valetStatusLock[j]);

            break;
        }
		else
		{
			/*valetStatusLock[j]->Release();*/
			ReleaseLock(valetStatusLock[j]);
		}
    }

    /*valetReadyWithCarLock[id]->Acquire();*/
	AcquireLock(valetReadyWithCarLock[id]);

    while(valetReadyWithCar[id]!=1)/* CHECKS IF THE VALET IS READY TO PARK HIS CAR*/
    {
            /* THE VALET IS NOT READY TO PARK THE CARS SO HE WAITS FOR THE VALET TO ARRIVE*/
            /*CV_valetReadyWithCar[id]->Wait(valetReadyWithCarLock[id]);*/
			WaitCondition(CV_valetReadyWithCar[id],valetReadyWithCarLock[id]);

    }
    /* THE VALET HAS ARRIVED */

    /*valetReadyWithCarLock[id]->Release();*/
	ReleaseLock(valetReadyWithCarLock[id]);

    /*visitorPleaseGetOutLock[id]->Acquire();*/
	AcquireLock(visitorPleaseGetOutLock[id]);

    visitorPleaseGetOut[id]=1;
    /* BROADCASTS THE VISITORS IN THE CAR TO GET OUT*/

    /*CV_visitorPleaseGetOut[id]->Broadcast(visitorPleaseGetOutLock[id]);*/
	BroadcastCondition(CV_visitorPleaseGetOut[id],visitorPleaseGetOutLock[id]);

    /*visitorPleaseGetOutLock[id]->Release();*/
	ReleaseLock(visitorPleaseGetOutLock[id]);

    /*visitorCountLock[id]->Acquire();*/
	AcquireLock(visitorCountLock[id]);

    if(visitorCount[id]!=0) /* CHECK IF ALL THE VISITORS HAVE GOT OUT */
    {
        /* NO STILL SOME ARE IN THE CAR SO HE WAITS */
        /*CV_visitorCount[id]->Wait(visitorCountLock[id]);*/
		WaitCondition(CV_visitorCount[id],visitorCountLock[id]);
    }
    /* OK NOW NO ONE IS THE CAR EXCEPT HIM*/
    /*visitorCountLock[id]->Release();*/
	ReleaseLock(visitorCountLock[id]);

    /*readyToParkLock[id]->Acquire();*/
	AcquireLock(readyToParkLock[id]);

    readyToPark[id]=1; /* INDICATES THE CAR IS READY TO PARK */

    /*CV_waitingToParkCar[id]->Signal(readyToParkLock[id]); //SIGNALS THE DRIVER TO PARK THE CAR*/
	SignalCondition(CV_waitingToParkCar[id],readyToParkLock[id]);

    /*readyToParkLock[id]->Release();*/
	ReleaseLock(readyToParkLock[id]);

    /* DRIVER IS GETTING OUT */

    /*printf("\n Driver %d is gettting out of the car %d and giving the keys to Valet ",id,id); */
	Write("\nDriver is getting out of the car and giving the keys to Valet",62,1);

	/*parkingTokenLock[id]->Acquire();*/
	AcquireLock(parkingTokenLock[id]);

	while(parkingToken[id]!=id)
	{
		/*printf("\n Driver %d is waiting to get his parking token from the Valet",id);*/
		Write("\nDriver is waiting to get his parking token from the Valet",58,1);

		/*CV_parkingToken[id]->Wait(parkingTokenLock[id]);*/
		WaitCondition(CV_parkingToken[id],parkingTokenLock[id]);
	}
	myParkingToken=parkingToken[id];
	/*printf("\n Driver %d got his parking token and the number on it is %d",id,myParkingToken);*/
	Write("\nDriver got parking token",25,1);

	/*parkingTokenLock[id]->Release();*/
	ReleaseLock(parkingTokenLock[id]);

    /* GOIN TO THE MUSEUM */
    /*printf("\n Driver%d is goin to the museum",id);*/
	Write("\nDriver is going to the museum",30,1);

	/*- DRIVER GOIN TO MUSUEM CODE SEGMENT */
	/*-START OF DRVIER TICKET TAKER INTERACTION CODE-*/

	 /*totalNumberOfActiveTicketTakersLock->Acquire();*/
	 AcquireLock(totalNumberOfActiveTicketTakersLock);

	if(totalNumberOfActiveTicketTakers==0)
	{
		/*printf("\n Driver %d is waiting since there are no active ticker takers ", id);*/
		Write("\nDriver is waiting since there are no active ticker takers",58,1);

        /*CV_noTicketTakers->Wait(totalNumberOfActiveTicketTakersLock);*/
			/*itoa(buf,10,CV_noTicketTakers);
			Write(buf,10,1);
			itoa(buf,10,totalNumberOfActiveTicketTakersLock);
			Write(buf,10,1);*/

			WaitCondition(CV_noTicketTakers,totalNumberOfActiveTicketTakersLock);
	}
	Write("1234",4,1);
	for(j=0;j<totalNumberOfActiveTicketTakers;j++)
	{
		/*totalNumberOfActiveTicketTakersLock->Release();*/
		ReleaseLock(totalNumberOfActiveTicketTakersLock);

		/*ticketTakerQueueLengthLock[j]->Acquire();*/
		AcquireLock(ticketTakerQueueLengthLock[j]);

		if(ticketTakerQueueLength[j]==0)
		{
			myTicketTaker=j;
			/*ticketTakerQueueLengthLock[j]->Release();*/
			ReleaseLock(ticketTakerQueueLengthLock[j]);
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
		/*ticketTakerQueueLengthLock[j]->Release();*/
		ReleaseLock(ticketTakerQueueLengthLock[j]);

		/*totalNumberOfActiveTicketTakersLock->Acquire();*/
		AcquireLock(totalNumberOfActiveTicketTakersLock);
	}
	/*totalNumberOfActiveTicketTakersLock->Release();*/
	ReleaseLock(totalNumberOfActiveTicketTakersLock);

	/*ticketTakerQueueLengthLock[myTicketTaker]->Acquire();*/
	AcquireLock(ticketTakerQueueLengthLock[myTicketTaker]);

	ticketTakerQueueLength[myTicketTaker]++;

	/*visitorMuseumEntryStatusLock[myTicketTaker]->Acquire();*/
	AcquireLock(visitorMuseumEntryStatusLock[myTicketTaker]);

	/*CV_waitingTicketTaker[myTicketTaker]->Signal(ticketTakerQueueLengthLock[myTicketTaker]);*/
	SignalCondition(CV_waitingTicketTaker[myTicketTaker],ticketTakerQueueLengthLock[myTicketTaker]);

	/*ticketTakerQueueLengthLock[myTicketTaker]->Release();*/
	ReleaseLock(ticketTakerQueueLengthLock[myTicketTaker]);

	if(visitorMuseumEntryStatus[myTicketTaker]!=1)
	{
			/*printf("\n Driver %d is waiting in the line of Ticket Taker %d",id,myTicketTaker);*/
		    /*CV_visitorMuseumEntryStatus[myTicketTaker]->Wait(visitorMuseumEntryStatusLock[myTicketTaker]);*/
			WaitCondition(CV_visitorMuseumEntryStatus[myTicketTaker],visitorMuseumEntryStatusLock[myTicketTaker]);
	}

	visitorMuseumEntryStatus[myTicketTaker]=0;

	/*visitorMuseumEntryStatusLock[myTicketTaker]->Release();*/
	ReleaseLock(visitorMuseumEntryStatusLock[myTicketTaker]);

	/*printf("\n Driver %d got his ticket accepdted ",id);*/
	Write("\nDriver got his ticket accepted",31,1);

	/*printf("\n Driver %d is inside the museum",id);*/
	Write("\nDriver is inside the musuem",28,1);

	/*----------END OF DRIVER TICKET TAKER INTERACTION CODE */
	/*---------- DRIVER GOIN TO MUSUEM CODE SEGMENT -- END --*/

	/*----------- DRIVER IS COMING BACK -----*/
	/*currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();
	currentThread->Yield();*/
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	Yield();
	/*printf("\n Driver %d says ok im done seeing the museum im goin back to get my car \n ",id);*/
	Write("\nDriver says im done seeing the museum I am going back to get my car",68,1);

	/*driverStatusLock[id]->Acquire();*/
	AcquireLock(driverStatusLock[id]);

	driverStatus[id]=1;

	/*visitorsReadyToGoHomeLock[id]->Acquire();*/
	AcquireLock(visitorsReadyToGoHomeLock[id]);

	/*printf("\n Driver %d is checking if all the visitos have come or not \n",id);*/
	Write("\nDriver is checking if all visitors have come or not",52,1);

	while(visitorsReadyToGoHome[id]<totalNumberOfVisitors[id])
	{
		/*visitorsReadyToGoHomeLock[id]->Release();*/
		ReleaseLock(visitorsReadyToGoHomeLock[id]);
		/*printf("\n Driver %d is saying I am waiting for the visitos in my car to arrive \n",id);*/
		Write("\nDriver says im waiting for my visitors to arrive",49,1);

		/*CV_driverStatus[id]->Wait(driverStatusLock[id]);*/
		WaitCondition(CV_driverStatus[id],driverStatusLock[id]);

		/*visitorsReadyToGoHomeLock[id]->Acquire();*/
		AcquireLock(visitorsReadyToGoHomeLock[id]);
	}
	/*visitorsReadyToGoHomeLock[id]->Release();*/
	ReleaseLock(visitorsReadyToGoHomeLock[id]);

	/*driverStatusLock[id]->Release();*/
	ReleaseLock(driverStatusLock[id]);

	/*printf("\n Driver %d says ok all the visitors in  my car has arrived im goin to start my interaction with the valet now \n",id);*/
	Write("\nDriver says ok all the visitors in  my car has arrived im going to start my interaction with the valet now",107,1);

	
	/*SHUD RITE CODE HERE TO INFORM THE VALET*/
	/* DRIVER IS COMING BACK END*/
	/*exitCarQueueLengthLock->Acquire();*/
	AcquireLock(exitCarQueueLengthLock);
	exitCarQueueLength++;
    /*printf("\n Number of waiting cars to exit is %d",exitCarQueueLength);*/
    /*waitingValetCountLock->Acquire();*/
	AcquireLock(waitingValetCountLock);

    if(countOfWaitingValets>0) /* CHECKS IF ANY VALET IS IN BENCH */
    {
        /*CV_countOfWaitingValets->Signal(waitingValetCountLock);*/
		SignalCondition(CV_countOfWaitingValets,waitingValetCountLock);
    }
    /*waitingValetCountLock->Release();*/
	ReleaseLock(waitingValetCountLock);
    /*CV_exitCarQueueLength->Wait(exitCarQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED*/
	WaitCondition(CV_exitCarQueueLength,exitCarQueueLengthLock);

    /*exitCarQueueLengthLock->Release();*/
	ReleaseLock(exitCarQueueLengthLock);

    for(j=0;j<totalNumberOfValets;j++)
    {
        /* SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE*/
        /*valetStatusLock[j]->Acquire();*/
		AcquireLock(valetStatusLock[j]);
        if(valetStatus[j]==-4)
        {
            /*ONE IS FOUND TO BE FREE*/
            valetStatus[j]=myParkingToken;
            /* CAR DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM */
            /*CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);*/
			SignalCondition(CV_valetWaitingForDriver[j],valetStatusLock[j]);
            /*valetStatusLock[j]->Release();*/
			ReleaseLock(valetStatusLock[j]);
            break;
        }
		else
		{
			/*valetStatusLock[j]->Release();*/
			ReleaseLock(valetStatusLock[j]);
		}
    }
	/*carReadyToGoLock[id]->Acquire();*/
	AcquireLock(carReadyToGoLock[id]);
	while(carReadyToGo[id]!=1)
	{
		/*printf("\n Driver %d says im waiting for the valet to get my car \n ", id);*/
		Write("\nDriver says im waiting for valet to get my car",47,1);

		/*CV_carReadyToGo[id]->Wait(carReadyToGoLock[id]);*/
		WaitCondition(CV_carReadyToGo[id],carReadyToGoLock[id]);
	}
	/*printf("\n Driver %d says the valet has come back with my car \n",id);*/
	Write("\nDriver says the valet has come back with my car",48,1);

	/*carReadyToGoLock[id]->Release();*/
	ReleaseLock(carReadyToGoLock[id]);

	/*tipsForThisCarLock[id]->Acquire();*/
	AcquireLock(tipsForThisCarLock[id]);
	
	/*printf("\n Driver %d says i am giving tip to the valet \n",id);*/
	Write("\nDriver says im giving tip to valet",35,1);

	tipsForThisCar[id]=1;
	/*CV_tipsForThisCar[id]->Signal(tipsForThisCarLock[id]);*/
	SignalCondition(CV_tipsForThisCar[id],tipsForThisCarLock[id]);

	/*tipsForThisCarLock[id]->Release();*/
	ReleaseLock(tipsForThisCarLock[id]);

	/*carKeysReadyLock[id]->Acquire();*/
	AcquireLock(carKeysReadyLock[id]);

	while(carKeysReady[id]!=1)
	{
		/*printf("\n Driver %d is waiting to get his keys \n",id);*/
		Write("\nDriver is waiting to get back the keys",39,1);
		/*CV_carKeysReady[id]->Wait(carKeysReadyLock[id]);*/
		WaitCondition(CV_carKeysReady[id],carKeysReadyLock[id]);
	}
	/*printf("\n Driver %d say i have the key to my car \n",id);*/
	Write("\nDriver says I have the keys to my car",38,1);

	/*carKeysReadyLock[id]->Release();*/
	ReleaseLock(carKeysReadyLock[id]);

	/*visitorPleaseGetInLock[id]->Acquire();*/
	AcquireLock(visitorPleaseGetInLock[id]);

	visitorPleaseGetIn[id]=1;

	/*visitorPleaseGetInLock[id]->Release();*/
	ReleaseLock(visitorPleaseGetInLock[id]);

	/*visitorsReadyToGoHomeLock[id]->Acquire();*/
	AcquireLock(visitorsReadyToGoHomeLock[id]);

	/*CV_visitorReadyToGoHome[id]->Broadcast(visitorsReadyToGoHomeLock[id]);*/
	BroadcastCondition(CV_visitorReadyToGoHome[id],visitorsReadyToGoHomeLock[id]);

	/*visitorCountLock[id]->Acquire();*/
	AcquireLock(visitorCountLock[id]);

	/*visitorsReadyToGoHomeLock[id]->Release();*/
	ReleaseLock(visitorsReadyToGoHomeLock[id]);
	while(visitorCount[id]<totalNumberOfVisitors[id])
	{
		/*CV_visitorPleaseGetIn[id]->Wait(visitorCountLock[id]);*/
		WaitCondition(CV_visitorPleaseGetIn[id],visitorCountLock[id]);
	}
	/*visitorCountLock[id]->Release();*/
	ReleaseLock(visitorCountLock[id]);

	/*printf("\n Driver %d says to all the vistors in his car that we are all going out of the museum",id);*/
	Write("\nDriver says we are all going out",33,1);
	Exit(0);
}

void visitorFunction()
{

    int id;/* RECORS THEIR IDENTITY*/
    int myTicketTaker=-1; /*INTIALLY THE VISITOR IS NOT WITH ANY TICKET TAKER*/
	int leastLength;
	int myCar;
	int j=0;
	char buf[2];
	AcquireLock(visitorIDLock);
	id=visitorID;
	visitorID++;
	ReleaseLock(visitorIDLock);
    myCar=visitor[id]; /* RECORDS THE CAR TO WHICH THEY BELONG*/
	/*itoa(buf,2,myCar);
	Write("\nVVVV",5,1);
	Write(buf,2,1);*/

	/*char *vehicle=new char[10];
	if(myCar<totalNumberOfCars)
	{
		sprintf(vehicle,"Car%d",myCar);
	}
	else
	{
		sprintf(vehicle,"Limousine%d",myCar);
	}*/

    /*visitorPleaseGetOutLock[myCar]->Acquire();*/
	AcquireLock(visitorPleaseGetOutLock[myCar]);
    if(visitorPleaseGetOut[myCar]!=1) /* CHECKS IF THERE IS ANY INDICATION FROM THE DRIVER ASKING THEM TO GET OUT OF THE CAR*/
    {
        /* NO THERE IS NO INDICATION SO THE VISITOR IS WAITING*/
        /*printf("\n Visitor %d is waiting to get out of the %s \n",id,vehicle);*/
		Write("\nVisitor is waiting to get out the car",38,1);

        /*CV_visitorPleaseGetOut[myCar]->Wait(visitorPleaseGetOutLock[myCar]);*/
		WaitCondition(CV_visitorPleaseGetOut[myCar],visitorPleaseGetOutLock[myCar]);
		/*Write("VVVV",4,1);*/
    }
    /*visitorPleaseGetOutLock[myCar]->Release();*/
	ReleaseLock(visitorPleaseGetOutLock[myCar]);
    /* OK NOW THEY HAVE RECIVED AN INDICATION FROM THE DRIVER*/

    /*visitorCountLock[myCar]->Acquire();*/
	AcquireLock(visitorCountLock[myCar]);

    visitorCount[myCar]--; /*SINCE THEY ARE GETTING OUT THEY ARE DECREMENTING THE COUNT OF VISITORS IN CAR */
    if(visitorCount[myCar]==0) /*/ CHECKS IF THE VISITOR IS THE LAST VISITOR*/
    {
        /* YES THE VISITOR IS THE LASR VISITOR SO HE SIGNALS THE DRIVER */
        /*CV_visitorCount[myCar]->Signal(visitorCountLock[myCar]);*/
		SignalCondition(CV_visitorCount[myCar],visitorCountLock[myCar]);
        /*printf("\n Visitor %d signaling his driver that everybody is out of the car \n",id);*/
		Write("\nVisitor signaling his driver that everybody is out of the car",62,1);
    }
    /*printf("\n Visitor %d is goin to the museum \n",id);*/
	Write("\nVisitor is going to the museum",31,1);
   /*visitorCountLock[myCar]->Release();*/
   ReleaseLock(visitorCountLock[myCar]);
   /*-----------------START OF   VISITOR TICKET TAKER INTERACTION CODE-------------------------------*/
	 /*totalNumberOfActiveTicketTakersLock->Acquire();*/
	 AcquireLock(totalNumberOfActiveTicketTakersLock);
	 if(totalNumberOfActiveTicketTakers==0)
	{
		/*printf("\n Visitor %d is waiting since there are no active Ticker Takers ", id);*/
		Write("\nVisitor is waiting since there is no ticket taker",50,1);
        /*CV_noTicketTakers->Wait(totalNumberOfActiveTicketTakersLock);*/
		WaitCondition(CV_noTicketTakers,totalNumberOfActiveTicketTakersLock);
	}
	for(j=0;j<totalNumberOfActiveTicketTakers;j++)
	{
		/*totalNumberOfActiveTicketTakersLock->Release();*/
		ReleaseLock(totalNumberOfActiveTicketTakersLock);

		/*ticketTakerQueueLengthLock[j]->Acquire();*/
		AcquireLock(ticketTakerQueueLengthLock[j]);
		if(ticketTakerQueueLength[j]==0)
		{
			myTicketTaker=j;
			/*ticketTakerQueueLengthLock[j]->Release();*/
			ReleaseLock(ticketTakerQueueLengthLock[j]);
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
	/*ticketTakerQueueLengthLock[j]->Release();*/
	ReleaseLock(ticketTakerQueueLengthLock[j]);
	/*totalNumberOfActiveTicketTakersLock->Acquire();*/
	AcquireLock(totalNumberOfActiveTicketTakersLock);
	}
	/*totalNumberOfActiveTicketTakersLock->Release();*/
	ReleaseLock(totalNumberOfActiveTicketTakersLock);

	/*ticketTakerQueueLengthLock[myTicketTaker]->Acquire();*/
	AcquireLock(ticketTakerQueueLengthLock[myTicketTaker]);

	ticketTakerQueueLength[myTicketTaker]++;

	/*visitorMuseumEntryStatusLock[myTicketTaker]->Acquire();*/
	AcquireLock( visitorMuseumEntryStatusLock[myTicketTaker]);

	/*CV_waitingTicketTaker[myTicketTaker]->Signal(ticketTakerQueueLengthLock[myTicketTaker]);*/
	SignalCondition(CV_waitingTicketTaker[myTicketTaker],ticketTakerQueueLengthLock[myTicketTaker]);

	/*ticketTakerQueueLengthLock[myTicketTaker]->Release();*/
	ReleaseLock(ticketTakerQueueLengthLock[myTicketTaker]);

	if(visitorMuseumEntryStatus[myTicketTaker]!=1)
	{
			/*printf("\n Visitor %d is waiting in the line of  Ticket Taker %d",id,myTicketTaker);*/
		    /*CV_visitorMuseumEntryStatus[myTicketTaker]->Wait(visitorMuseumEntryStatusLock[myTicketTaker]);*/
			WaitCondition(CV_visitorMuseumEntryStatus[myTicketTaker],visitorMuseumEntryStatusLock[myTicketTaker]);
	}
	visitorMuseumEntryStatus[myTicketTaker]=0;

	/*visitorMuseumEntryStatusLock[myTicketTaker]->Release();*/
	ReleaseLock(visitorMuseumEntryStatusLock[myTicketTaker]);

	/*printf("\n Visitor %d got his ticket accepdted ",id);*/
	Write("\nVisitor got his ticket accepted",32,1);

	/*printf("\n Visitor %d is inside the museum",id);*/
	Write("\nVisitor is inside the museum",29,1);
	/*-------------------END OF VISITOR TICKET TAKER INTERACTION CODE --------------------------------------*/
	/*currentThread->Yield();
	currentThread->Yield();*/
	Yield();
	Yield();
	Yield();
	/*printf("\n Visitor %d say ok fine im done seeing the museum im goin back to my car \n",id);*/
	Write("\nVisitor says ok im done seeing the museum im going back to my car",66,1);
	/* ALRIGHT THE VISITORS ARE WAITING TO GO HOME  */

	/*visitorsReadyToGoHomeLock[myCar]->Acquire();*/
	AcquireLock(visitorsReadyToGoHomeLock[myCar]);

	visitorsReadyToGoHome[myCar]++;
	/*printf("\n %d out of %d ppl are back now \n",visitorsReadyToGoHome[myCar],totalNumberOfVisitors[myCar]);*/
	if(visitorsReadyToGoHome[myCar]<totalNumberOfVisitors[myCar])
	{
				/*CV_visitorReadyToGoHome[myCar]->Wait(visitorsReadyToGoHomeLock[myCar]);*/
				WaitCondition(CV_visitorReadyToGoHome[myCar],visitorsReadyToGoHomeLock[myCar]);
	}
	else
	{
		/*printf("\n the total number of visitors waiting to get back on the car %d is %d \n",myCar,visitorsReadyToGoHome[myCar]);*/
		/*driverStatusLock[myCar]->Acquire();*/
		AcquireLock(driverStatusLock[myCar]);
		/*printf("\n Visitor %d is checking if Driver has arrived \n",id);*/
		Write("\nVisitor is checking if driver has arrived",42,1);

		if(driverStatus[myCar]==1)
		{
			/*printf("\n Visitor %d says yes the driver is already there \n",id);*/
			Write("\nVisitor says the driver is already there",41,1);

			/*CV_driverStatus[myCar]->Signal(driverStatusLock[myCar]);*/
			SignalCondition(CV_driverStatus[myCar],driverStatusLock[myCar]);
			/*driverStatusLock[myCar]->Release();*/
			ReleaseLock(driverStatusLock[myCar]);
			/*CV_visitorReadyToGoHome[myCar]->Wait(visitorsReadyToGoHomeLock[myCar]);*/
			WaitCondition(CV_visitorReadyToGoHome[myCar],visitorsReadyToGoHomeLock[myCar]);
		}
		else
		{
			/*driverStatusLock[myCar]->Release();*/
			ReleaseLock(driverStatusLock[myCar]);
			/*printf("\n Visitor %d says nope still no driver I have to wait \n",id);*/
			Write("\nVisitor says nope still no driver",34,1);

			/*CV_visitorReadyToGoHome[myCar]->Wait(visitorsReadyToGoHomeLock[myCar]);*/
			WaitCondition(CV_visitorReadyToGoHome[myCar],visitorsReadyToGoHomeLock[myCar]);
		}

	}
	/*visitorsReadyToGoHomeLock[myCar]->Release();*/
	ReleaseLock(visitorsReadyToGoHomeLock[myCar]);

	/*visitorCountLock[myCar]->Acquire();*/
	AcquireLock(visitorCountLock[myCar]);
	visitorCount[myCar]++;
	if(visitorCount[myCar]==totalNumberOfVisitors[myCar])
	{
		/*printf("\n the count of visitors in the car %d and the total count is %d",visitorCount[myCar],totalNumberOfVisitors[myCar]);*/
		/*printf("\n Visitor %d is signaling driver we are all in ",id);*/
		Write("\nVisitor is Signaling the driver that we are all done",53,1);
		/*CV_visitorPleaseGetIn[myCar]->Signal(visitorCountLock[myCar]);*/
		SignalCondition(CV_visitorPleaseGetIn[myCar],visitorCountLock[myCar]);
	}
	/*visitorCountLock[myCar]->Release();*/
	ReleaseLock(visitorCountLock[myCar]);
	Exit(0);
}

void limoDriver()
{
	
    int id=-1; /* DRIVER RECORDS HIS IDENTITY*/
	int myParkingToken=-1;
	int j=0;
	AcquireLock(limoDriverIDLock);
	id=limoDriverID;
	limoDriverID++;
	ReleaseLock(limoDriverIDLock);

    /*limoQueueLengthLock->Acquire();*/
	AcquireLock(limoQueueLengthLock);
    limoQueueLength++; /* HE INCREMENTS THE CAR QUEUE LENGTH*./
    /*printf("\n Limousine %d is waitig in line and the number of waiting Limousines is %d",id,limoQueueLength);*/
	Write("\nLimousine driver waiting in Line",33,1);
    /*waitingValetCountLock->Acquire();*/
	AcquireLock(waitingValetCountLock);
    if(countOfWaitingValets>0) /* CHECKS IF ANY VALET IS IN BENCH */
    {
        /* THERE IS ONE FOUND IN THE BENCH SO THE DRIVER SIGNALS HIM */
		/*CV_countOfWaitingValets->Signal(waitingValetCountLock);*/
		SignalCondition(CV_countOfWaitingValets,waitingValetCountLock);
    }
    /*waitingValetCountLock->Release();*/
	ReleaseLock(waitingValetCountLock);
	/*CV_limosWaitingToGetParked->Wait(limoQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED*/
	WaitCondition(CV_limosWaitingToGetParked,limoQueueLengthLock);
    /*limoQueueLengthLock->Release();*/
	ReleaseLock(limoQueueLengthLock);
    /* ITS THE DRIVER CHANCE NOW */
    for(j=0;j<totalNumberOfValets;j++)
    {
        /* SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE */
        /*valetStatusLock[j]->Acquire();*/
		AcquireLock(valetStatusLock[j]);

        if(valetStatus[j]==-1)
        {
            /* ONE IS FOUND TO BE FREE */
			 valetStatus[j]=id;
            /* CAR DRIVER ASSOCIATED HIMSELF WITH HIM AND SIGNALS HIM */
            /*CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);*/
			SignalCondition(CV_valetWaitingForDriver[j],valetStatusLock[j]);

            /*valetStatusLock[j]->Release();*/
			ReleaseLock(valetStatusLock[j]);
            break;
        }
        /*valetStatusLock[j]->Release();*/
		ReleaseLock(valetStatusLock[j]);

    }
    /*valetReadyWithCarLock[id]->Acquire();*/
	AcquireLock(valetReadyWithCarLock[id]);
    while(valetReadyWithCar[id]!=1)/* CHECKS IF THE VALET IS READY TO PARK HIS CAR */
    {
            /*THE VALET IS NOT READY TO PARK THE CARS SO HE WAITS FOR THE VALET TO ARRIVE*/
            /*CV_valetReadyWithCar[id]->Wait(valetReadyWithCarLock[id]);*/
			WaitCondition(CV_valetReadyWithCar[id],valetReadyWithCarLock[id]);

    }
    /* THE VALET HAS ARRIVED */
    /*valetReadyWithCarLock[id]->Release();*/
	ReleaseLock(valetReadyWithCarLock[id]);
    /*visitorPleaseGetOutLock[id]->Acquire();*/
	AcquireLock(visitorPleaseGetOutLock[id]);
    visitorPleaseGetOut[id]=1;
    /* BROADCASTS THE VISITORS IN THE CAR TO GET OUT */
    /*CV_visitorPleaseGetOut[id]->Broadcast(visitorPleaseGetOutLock[id]);*/
	BroadcastCondition(CV_visitorPleaseGetOut[id],visitorPleaseGetOutLock[id]);

    /*visitorPleaseGetOutLock[id]->Release();*/
	ReleaseLock(visitorPleaseGetOutLock[id]);

    /*visitorCountLock[id]->Acquire();*/
	AcquireLock(visitorCountLock[id]);

    if(visitorCount[id]!=0)/* CHECK IF ALL THE VISITORS HAVE GOT OUT */
    {
        /* NO STILL SOME ARE IN THE CAR SO HE WAITS */
        /*CV_visitorCount[id]->Wait(visitorCountLock[id]);*/
		WaitCondition(CV_visitorCount[id],visitorCountLock[id]);
    }
    /* OK NOW NO ONE IS THE CAR EXCEPT HIM */
    /*visitorCountLock[id]->Release();*/
	ReleaseLock(visitorCountLock[id]);

    /*readyToParkLock[id]->Acquire();*/
	AcquireLock(readyToParkLock[id]);

    readyToPark[id]=1; /* INDICATES THE CAR IS READY TO PARK */

    /*CV_waitingToParkCar[id]->Signal(readyToParkLock[id]); //SIGNALS THE DRIVER TO PARK THE CAR*/
	SignalCondition(CV_waitingToParkCar[id],readyToParkLock[id]);

    /*readyToParkLock[id]->Release();*/
	ReleaseLock(readyToParkLock[id]);

    /* DRIVER IS GETTING OUT*/
    /*printf("\n Limousine driver %d is gettting out of the Limousine %d and giving the keys to Valet ",id,id);*/
	Write("\nLimousine driver  is getting out of the Limousine  and giving the keys to Valet",80,1);

    /* GOIN TO THE MUSEUM*/
	/*parkingTokenLock[id]->Acquire();*/
	AcquireLock(parkingTokenLock[id]);

	while(parkingToken[id]!=id)
	{
		/*printf("\n Limousine driver %d is waiting to get his parking token from the Valet",id);*/
		Write("\nLimousine driver is waiting to get his parking token from the Valet",68,1);

		/*CV_parkingToken[id]->Wait(parkingTokenLock[id]);*/
		WaitCondition(CV_parkingToken[id],parkingTokenLock[id]);
	}
	myParkingToken=parkingToken[id];
	/*printf("\n Limousine driver %d got his parking token and the number on it is %d",id,myParkingToken);*/
	Write("\nLimousine driver got his parking token",39,1);

	/*parkingTokenLock[id]->Release();*/
	ReleaseLock(parkingTokenLock[id]);

	/*------ LIMO DRIVER WAITING FOR VISITORS AND INTERACTING WITH VALET TO GET HIS CAR BACK ----------*/

	/*driverStatusLock[id]->Acquire();*/
	AcquireLock(driverStatusLock[id]);

	driverStatus[id]=1;

	/*visitorsReadyToGoHomeLock[id]->Acquire();*/
	AcquireLock(visitorsReadyToGoHomeLock[id]);

	/*printf("\n Limousine driver %d is checking if all the visitos have come or not ",id);*/
	Write("\nLimousine driver is checking if all the visitors have come or not",68,1);

	while(visitorsReadyToGoHome[id]<totalNumberOfVisitors[id])
	{
		/*visitorsReadyToGoHomeLock[id]->Release();*/
		ReleaseLock(visitorsReadyToGoHomeLock[id]);

		/*printf("\n Limousine driver %d is saying I am waiting for the visitors in my car to arrive \n",id);*/
		Write("\nLimousine driver  is saying I am waiting for the visitors in my car to arrive",78,1);

		/*CV_driverStatus[id]->Wait(driverStatusLock[id]);*/
		WaitCondition(CV_driverStatus[id],driverStatusLock[id]);

		/*visitorsReadyToGoHomeLock[id]->Acquire();*/
		AcquireLock(visitorsReadyToGoHomeLock[id]);
	}
	/*visitorsReadyToGoHomeLock[id]->Release();*/
	ReleaseLock(visitorsReadyToGoHomeLock[id]);
	/*driverStatusLock[id]->Release();*/
	ReleaseLock(driverStatusLock[id]);
	

	/*printf("\n Limousine driver %d says ok all the visitors in  my car has arrived im goin to start my interaction with the valet now \n",id);*/
	Write("\nLimousine driver  says ok all the visitors in  my car has arrived im goin to start my interaction with the valet now",117,1);

	
	/* SHUD RITE CODE HERE TO INFORM THE VALET */
	/*------ DRIVER IS COMING BACK END --------*/
	/* SHOULD CONTINUE FROM HERE */

	/*exitLimoQueueLengthLock->Acquire();*/
	AcquireLock(exitLimoQueueLengthLock);
	exitLimoQueueLength++;
    /*printf("\n Number of waiting Limousines to exit is is %d",exitLimoQueueLength);*/
    /*waitingValetCountLock->Acquire();*/
	AcquireLock(waitingValetCountLock);

    if(countOfWaitingValets>0) /* CHECKS IF ANY VALET IS IN BENCH */
    {
       /*CV_countOfWaitingValets->Signal(waitingValetCountLock);*/
	   SignalCondition(CV_countOfWaitingValets,waitingValetCountLock);
    }
    /*waitingValetCountLock->Release();*/
	ReleaseLock(waitingValetCountLock);

    /*CV_exitLimoQueueLength->Wait(exitLimoQueueLengthLock); // DRIVERS SLEEPS TILL HIS CHANCE COMES TO GET SERVICED*/
	WaitCondition(CV_exitLimoQueueLength,exitLimoQueueLengthLock);

    /*exitLimoQueueLengthLock->Release();*/
	ReleaseLock(exitLimoQueueLengthLock);

    for(j=0;j<totalNumberOfValets;j++)
    {
        /* SCANS THE STATUS OF ALL THE VALETS TO SEE IF ANY ONE IS FREE */
        /*valetStatusLock[j]->Acquire();*/
		AcquireLock(valetStatusLock[j]);
        if(valetStatus[j]==-3)
        {
            
            valetStatus[j]=myParkingToken;

            /*CV_valetWaitingForDriver[j]->Signal(valetStatusLock[j]);*/
			SignalCondition(CV_valetWaitingForDriver[j],valetStatusLock[j]);
            
			/*valetStatusLock[j]->Release();*/
			ReleaseLock(valetStatusLock[j]);
            break;
        }
        /*valetStatusLock[j]->Release();*/
		ReleaseLock(valetStatusLock[j]);
    }

	/*carReadyToGoLock[id]->Acquire();*/
	AcquireLock(carReadyToGoLock[id]);

	while(carReadyToGo[id]!=1)
	{
		/*printf("\n Limousine driver %d is waiting for the Valet to get his car \n ", id);*/
		Write("\nLimousine driver is waiting for the Valet to get his car",57,1);

		/*CV_carReadyToGo[id]->Wait(carReadyToGoLock[id]);*/
		WaitCondition(CV_carReadyToGo[id],carReadyToGoLock[id]);
	}
	/*printf("\n Limousine driver %d says the valet has come back with my car \n",id);*/
	Write("\nLimousine driver says the valet has come back with my car",58,1);

	/*carReadyToGoLock[id]->Release();*/
	ReleaseLock(carReadyToGoLock[id]);

	/*tipsForThisCarLock[id]->Acquire();*/
	AcquireLock(tipsForThisCarLock[id]);

	/*printf("\n Limousine driver %d says i am giving tip to the valet\n",id);*/
	Write("\nLimousine driver says i am giving tip to the valet",51,1);

	tipsForThisCar[id]=1;
	/*CV_tipsForThisCar[id]->Signal(tipsForThisCarLock[id]);*/
	SignalCondition(CV_tipsForThisCar[id],tipsForThisCarLock[id]);

	/*tipsForThisCarLock[id]->Release();*/
	ReleaseLock(tipsForThisCarLock[id]);

	/*carKeysReadyLock[id]->Acquire();*/
	AcquireLock(carKeysReadyLock[id]);

	while(carKeysReady[id]!=1)
	{
		/*printf("\n Limousine driver %d is waiting to get his keys \n",id);*/
		Write("\nLimousine driver is waiting to get his keys",44,1);

		/*CV_carKeysReady[id]->Wait(carKeysReadyLock[id]);*/
		WaitCondition(CV_carKeysReady[id],carKeysReadyLock[id]);
	}
	/*printf("\n Limousine driver %d say i have the key to my car \n",id);*/
	Write("\nLimousine driver says i have the key to my car",47,1);

	/*carKeysReadyLock[id]->Release();*/
	ReleaseLock(carKeysReadyLock[id]);

	/*visitorPleaseGetInLock[id]->Acquire();*/
	AcquireLock(visitorPleaseGetInLock[id]);

	visitorPleaseGetIn[id]=1;

	/*visitorPleaseGetInLock[id]->Release();*/
	ReleaseLock(visitorPleaseGetInLock[id]);

	/*visitorsReadyToGoHomeLock[id]->Acquire();*/
	AcquireLock(visitorsReadyToGoHomeLock[id]);

	/*CV_visitorReadyToGoHome[id]->Broadcast(visitorsReadyToGoHomeLock[id]);*/
	BroadcastCondition(CV_visitorReadyToGoHome[id],visitorsReadyToGoHomeLock[id]);

	/*visitorCountLock[id]->Acquire();*/
	AcquireLock(visitorCountLock[id]);

	/*visitorsReadyToGoHomeLock[id]->Release();*/
	ReleaseLock(visitorsReadyToGoHomeLock[id]);

	while(visitorCount[id]<totalNumberOfVisitors[id])
	{
		/*CV_visitorPleaseGetIn[id]->Wait(visitorCountLock[id]);*/
		WaitCondition(CV_visitorPleaseGetIn[id],visitorCountLock[id]);
	}
	/*visitorCountLock[id]->Release();*/
	ReleaseLock(visitorCountLock[id]);

	/*printf("\n Limousine driver %d says to the all the vistors in his limousine that we are all going out of the museum",id);*/
	Write("\nLimousine driver says to the all the visitors in his limousine that we are all going out of the museum",103,1);
	/*-------------------------------------------------------------------------------------------------------------------------------------------//*/
  Exit(0); 
}

void valetManager()
{
	/*int id=i;*/
	int j=0;

	while(1)
	{
		/*printf("\n VM: the count of excited cars is %d",countOfExitedCars);
		//printf("\n VM: the count of total number of cars is %d",totalNumberOfCars);
		//printf("\n VM: the count of total number of limosine is %d",totalCountOfLimos);
		//printf("\n VM: the count of valets in back room is %d",(totalNumberOfValets-countOfWaitingValets));
		//printf("\n the valet Manager comes in ");
		//---------------------------- SHOULD SEND ONE TO THE BACK ROOM --------------------------*/
		/*currentThread->Yield();*/
		Yield();
		/*currentThread->Yield();*/
		Yield();
		/*currentThread->Yield();*/
		Yield();
		/*currentThread->Yield();*/
		Yield();
		/*currentThread->Yield();*/
		Yield();
		/*currentThread->Yield();*/
		Yield();

		/*waitingValetCountLock->Acquire();*/
		AcquireLock(waitingValetCountLock);
		if(countOfWaitingValets>=2)
		{
			/*valetManagerWaitingLock->Acquire();*/
			AcquireLock(valetManagerWaitingLock);
			/* SHOULD CONTINUE FROM HERE */

			valetManagerWaiting++;
			/*printf("\n Valet Manager is signaling a valet ");*/
			Write("\n Valet Manager is signaling a valet",36,1);
			
			/*CV_countOfWaitingValets->Signal(waitingValetCountLock);*/
			SignalCondition(CV_countOfWaitingValets,waitingValetCountLock);

			/*waitingValetCountLock->Release();*/
			ReleaseLock(waitingValetCountLock);

			/*printf(" \n Valet Manager is waiting for the valet to come to him ");*/
			Write("\nValet Manager is waiting for the valet to come to him",54,1);
			
			/*CV_valetManagerWaiting->Wait(valetManagerWaitingLock);*/
			WaitCondition(CV_valetManagerWaiting,valetManagerWaitingLock);

			/*valetManagerWaitingLock->Release();*/
			ReleaseLock(valetManagerWaitingLock);

			for(j=0;j<totalNumberOfValets;j++)
			{
				/*valetStatusLock[j]->Acquire();*/
				AcquireLock(valetStatusLock[j]);

				if(valetStatus[j]==-9)
				{
					valetStatus[j]=-10;
					/*printf("\n Valet Manager has asked valet %d to go to the back room ",j);*/
					Write("\nValet Manager has asked valet to go to the back room",53,1);

					/*CV_waitingForValetManager[j]->Signal(valetStatusLock[j]);*/
					SignalCondition(CV_waitingForValetManager[j],valetStatusLock[j]);

					/*valetStatusLock[j]->Release();*/
					ReleaseLock(valetStatusLock[j]);

					break;
				}
				else
				{
					/*valetStatusLock[j]->Release();*/
					ReleaseLock(valetStatusLock[j]);
				}
			}
		}
		else
		{
					/*waitingValetCountLock->Release();*/
					ReleaseLock(waitingValetCountLock);
		}
		/*-------- SHOULD WAKE ONE UP FROM THE BACK ROOM----------------*/
		/*limoQueueLengthLock->Acquire();*/
		AcquireLock(limoQueueLengthLock);

		/*carQueueLengthLock->Acquire();*/
		AcquireLock(carQueueLengthLock);

		if((limoQueueLength+carQueueLength)>=4)
		{
			/*limoQueueLengthLock->Release();*/
			ReleaseLock(limoQueueLengthLock);

			/*carQueueLengthLock->Release();*/
			ReleaseLock(carQueueLengthLock);

			for(j=0;j<totalNumberOfValets;j++)
			{
				/*valetStatusLock[j]->Acquire();*/
				AcquireLock(valetStatusLock[j]);
				if(valetStatus[j]==-10)
				{
					valetStatus[j]=-5;
					/*CV_waitingInBackRoom[j]->Signal(valetStatusLock[j]);*/
					SignalCondition(CV_waitingInBackRoom[j],valetStatusLock[j]);

					/*valetStatusLock[j]->Release();*/
					ReleaseLock(valetStatusLock[j]);
					break;
				}
				else
				{
					/*valetStatusLock[j]->Release();*/
					ReleaseLock(valetStatusLock[j]);
				}
			}
		}
		else
		{
			/*limoQueueLengthLock->Release();*/
			ReleaseLock(limoQueueLengthLock);
			/*carQueueLengthLock->Release();*/
			ReleaseLock(carQueueLengthLock);
		}
	}
 Exit(0);
}

int main()
{
	int i;
	int temp=totalNumberOfCars;
	char buf[10];
	Write("\nSimulation starting",20,1);
	valetIDLock=CreateLock();
	limoDriverIDLock=CreateLock();
	driverIDLock=CreateLock();
	visitorIDLock=CreateLock();
	ticketTakerIDLock=CreateLock();

	for(i=0; i<grandTotalOfVisitors; i++)
	{
		visitor[i]=i;
		visitorMuseumEntryStatus[i]=0;
		/*visitorMuseumEntryStatusLock[i]=-1;
		CV_visitorMuseumEntryStatus[i]=-1;*/
	}
	for(i=0;i<grandTotalOfVisitors;i++)
	{
		visitorMuseumEntryStatusLock[i]=CreateLock();
		CV_visitorMuseumEntryStatus[i]=CreateCondition();
	}

	/*for(i=0;i<totalNumberOfCars;i++)
	{
		totalNumberOfVisitors[i]=1;
	}
	for(i=totalNumberOfCars;i<totalNumberOfVehicles;i++)
	{
		totalNumberOfVisitors[i]=1;
	}*/

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		totalNumberOfVisitors[i]=1;
		totalNumberOfVisitorsLock[i]=CreateLock();
	}


	/*for(i=0;i<totalNumberOfValets;i++)
	{
		CV_waitingForValetManager[i]=-1;
		CV_waitingInBackRoom[i]=-1;
	}*/

	for(i=0;i<totalNumberOfValets;i++)
	{
		CV_waitingForValetManager[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfValets;i++)
	{
		CV_waitingInBackRoom[i]=CreateCondition();
	}

	valetManagerWaitingLock=CreateLock();
	CV_valetManagerWaiting=CreateCondition();

	for(i=0;i<totalNumberOfValets;i++)
	{
		valetStatus[i]=-5;
	}

	/*for(i=0;i<totalNumberOfValets;i++)
	{
		valetStatusLock[i]=-1
	}*/
	for(i=0;i<totalNumberOfValets;i++)
	{
		valetStatusLock[i]=CreateLock();
	}

	/*for(i=0;i<totalNumberOfValets;i++)
	{
		CV_valetWaitingForDriver[i]=-1;
	}*/
	for(i=0;i<totalNumberOfValets;i++)
	{
		CV_valetWaitingForDriver[i]=CreateCondition();
	}

	waitingValetCountLock=CreateLock();
	CV_countOfWaitingValets=CreateCondition();

	limoQueueLengthLock=CreateLock();
	CV_limosWaitingToGetParked=CreateCondition();

	carQueueLengthLock=CreateLock();
	CV_waitingToGetParked=CreateCondition();

	exitLimoQueueLengthLock=CreateLock();
	CV_exitLimoQueueLength=CreateCondition();

	exitCarQueueLengthLock=CreateLock();
	CV_exitCarQueueLength=CreateCondition();

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		readyToPark[i]=0;
		/*readyToParkLock[i]=-1;
		CV_waitingToParkCar[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		readyToParkLock[i]=CreateLock();
		CV_waitingToParkCar[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorPleaseGetOut[i]=0;
		/*visitorPleaseGetOutLock[i]=-1;
		CV_visitorPleaseGetOut[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorPleaseGetOutLock[i]=CreateLock();
		CV_visitorPleaseGetOut[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorPleaseGetIn[i]=0;
		/*visitorPleaseGetInLock[i]=-1;
		CV_visitorPleaseGetIn[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorPleaseGetInLock[i]=CreateLock();
		CV_visitorPleaseGetIn[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorCount[i]=1;
		/*visitorCountLock[i]=-1;
		CV_visitorCount[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorCountLock[i]=CreateLock();
		CV_visitorCount[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		parkingToken[i]=-1;
		/*parkingTokenLock[i]=-1;
		CV_parkingToken[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		parkingTokenLock[i]=CreateLock();
		CV_parkingToken[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		valetReadyWithCar[i]=0;
		/*valetReadyWithCarLock[i]=-1;
		CV_valetReadyWithCar[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		valetReadyWithCarLock[i]=CreateLock();
		CV_valetReadyWithCar[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorsReadyToGoHome[i]=0;
		/*visitorsReadyToGoHomeLock[i]=-1;
		CV_visitorReadyToGoHome[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		visitorsReadyToGoHomeLock[i]=CreateLock();
		CV_visitorReadyToGoHome[i]=CreateCondition();
	}


	for(i=0;i<totalNumberOfVehicles;i++)
	{
		driverStatus[i]=0;
		/*driverStatusLock[i]=-1;
		CV_driverStatus[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		driverStatusLock[i]=CreateLock();
		CV_driverStatus[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		tipsForThisCar[i]=0;
		/*tipsForThisCarLock[i]=-1;
		CV_tipsForThisCar[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		tipsForThisCarLock[i]=CreateLock();
		CV_tipsForThisCar[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		carKeysReady[i]=0;
		/*carKeysReadyLock[i]=-1;
		CV_carKeysReady[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		carKeysReadyLock[i]=CreateLock();
		CV_carKeysReady[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		carReadyToGo[i]=0;
		/*carReadyToGoLock[i]=-1;
		CV_carReadyToGo[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		carReadyToGoLock[i]=CreateLock();
		CV_carReadyToGo[i]=CreateCondition();
	}

	for(i=0;i<totalNumberOfVehicles;i++)
	{
		carParked[i]=0;
		/*carParkedLock[i]=-1;*/
	}
	for(i=0;i<totalNumberOfVehicles;i++)
	{
		carParkedLock[i]=CreateLock();
	}


	totalNumberOfActiveTicketTakersLock=CreateLock();
	CV_noTicketTakers=CreateCondition();
	/*itoa(buf,10,CV_noTicketTakers);
	Write(buf,10,1);*/

	ticketTakerLock=CreateLock();
	CV_ticketTaker=CreateCondition();

	
	for(i=0;i<totalNumberOfTicketTakers;i++)
	{
		ticketTakerQueueLength[i]=0;
		ticketTakerQueueLengthLock[i]=-1;
		CV_ticketTakerQueueLength[i]=-1;
		CV_waitingTicketTaker[i]=-1;
	}
	
	for(i=0;i<totalNumberOfTicketTakers;i++)
	{
		ticketTakerQueueLengthLock[i]=CreateLock();
		CV_ticketTakerQueueLength[i]=CreateCondition();
		CV_waitingTicketTaker[i]=CreateCondition();
	}

	totalNumberOfVisitorsAcceptedLock=CreateLock();
	
	for(i=0;i<totalNumberOfValets;i++)
	{
		Fork(valet);
	}
	for(i=0;i<totalNumberOfDrivers;i++)
	{
		Fork(driver);
	}
	for(i=0;i<totalCountOfLimoDrivers;i++)
	{
		Fork(limoDriver);
	}
	for(i=0;i<grandTotalOfVisitors;i++)
	{
		Fork(visitorFunction);
	}
	for(i=0;i<totalNumberOfTicketTakers;i++)
	{
		Fork(ticketTakerFunction);
	}
	Fork(valetManager);
	Exit(0);
}


