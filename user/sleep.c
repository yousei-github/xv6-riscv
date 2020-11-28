

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int main(int argc, char *argv[])
{
  int num;
	
	
  if(argc < 2)	// no parameter input
  {
  	num = 1;
    sleep(num);
  }
	else	// received a parameter at argv[1]
	{
		num = atoi(argv[argc-1]);
		sleep(num);
  }
  
  
  exit(0);
}


