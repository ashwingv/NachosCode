/*---------------------------------------------------------------------
                   testnetlock.c

- This program is used to test Createlock, Acquire lock, Release lock and 
Destroy lock in a client server envrionment

------------------------------------------------------------------------
*/

#include "syscall.h"
void main()
{
  unsigned int i;
  int lock,cond;
  lock = CreateLock("lock",4);
  AcquireLock(lock);
  cond=CreateCondition("condition",9);
  WaitCondition(cond,lock);
  ReleaseLock(lock);
  Exit(0);
}
