/*
testRpcLock1.c

This is the test case we are trying acquire 
release and destroy a lock which is not yet created.

*/
#include "syscall.h"
void main()
{
  unsigned int i;
  int LockNum=5;
  AcquireLock(LockNum);
  ReleaseLock(LockNum);
  DestroyLock(LockNum);
}
