/*
testRpcLock.c

This is the test case to check the correct working of locks
*/
#include "syscall.h"
void main()
{
  unsigned int i;
  int LockNum;
  Write("\nTest Create lock\n", 18,1 );
  LockNum = CreateLock("lock",4);
  ReleaseLock(LockNum);
  DestroyLock(LockNum);
  AcquireLock(LockNum);

}
