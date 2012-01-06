#include "syscall.h"
void main()
{
  unsigned int i;
  int conditionNum;

  conditionNum=CreateCondition("condition",9);
  DestroyCondition(conditionNum);
	
  Exit(0);
}
