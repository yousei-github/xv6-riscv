#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/param.h"



int main(int argc, char *argv[])
{
	int cpid;
	char command_buffer[MAXARG];
	char argv_buffer[MAXARG];
	char argv2_buffer[MAXARG];
	char *argv2[4];
	int buffer_length;
	
	// collect arguments
	if(argc < 2)	// no parameter input
  {
    printf("xargs: failed, need arguments\n");
  }
	else if(argc < 3)	// received a parameter at argv[1]
	{
		printf("xargs: failed, need more arguments\n");
  }
  else	// received two parameters at argv[1] and argv[2]
  {
		strcpy(command_buffer, argv[1]);
		strcpy(argv_buffer, argv[2]);
		
		while (1)	// read lines from standard input and run a command for each line
		{
			gets(argv2_buffer, MAXARG);
			
			buffer_length = strlen(argv2_buffer);
			
			if (buffer_length == 0)	// #1
				break;
			// delete the '\n' from the end of input sequence, and replace it with '\0'
			//argv2_buffer[strlen(argv2_buffer) - 1] = '\0';	// #2
			argv2_buffer[buffer_length - 1] = '\0';
			
			// Note: if don't write #1 but write #2, the error usertrap(): unexpected scause 0x000000000000000f pid=... will happen
			
			// load arguments
			argv2[0] = command_buffer;
			argv2[1] = argv_buffer;
			argv2[2] = argv2_buffer;
			argv2[3] = 0;	// end?
			
			cpid = fork();
			
			if (cpid > 0)	// Parent Process 
			{ 
				wait(0);	// parent waits for child
				//printf("exec succeed\n");
			} 
			else if (cpid == 0)	// Child Process 
			{ 
				exec(argv2[0], argv2);
				printf("exec error\n");
				break;
			} 
			else 
			{
				printf("Fork failed\n");
				break;
			}
		}
  }
  exit(0); 
}


