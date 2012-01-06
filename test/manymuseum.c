/*-------------------------------------------------------
In This test we are running multiple simulations of museum
-----------------------------------------------------------*/
#include "syscall.h"

int main()
{
	Write("\n Inside main",13,1);
	Write("\n Running museum1",18,1);
	Exec("../test/museum1",15);
	Write("\n Running museum3",18,1);
	Exec("../test/museum3",15);
	Exit(0);

}