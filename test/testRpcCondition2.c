
#include "syscall.h"
void main()
{
  unsigned int i;
  int lock,cond;
  Write("\nTest Create lock\n", 18,1 );
  lock = CreateLock("lock",4);
  AcquireLock(lock);
  cond=CreateCondition("condition",9);
  SignalCondition(cond,lock);
  ReleaseLock(lock);
  Exit(0);
}