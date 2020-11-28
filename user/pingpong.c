#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"



int main(int argc, char *argv[])
{
  int cpid, mypid;
  int parent_fd[2]; 
  int child_fd[2];
	char buffer;
	
	
	pipe(parent_fd);
  pipe(child_fd);
	//printf("parent_fd[0] is %d, parent_fd[1] is %d.\n", parent_fd[0], parent_fd[1]);
	//printf("child_fd[0] is %d, child_fd[1] is %d.\n", child_fd[0], child_fd[1]);
	
	cpid = fork();
	
	if (cpid > 0)	/* Parent Process */
	{ 
		mypid = getpid();
		// [1] the parent sends by writing a byte
		buffer = mypid;
		write(parent_fd[1], &buffer, sizeof(buffer));
		
		
		// [4] the parent then reads
		read(child_fd[0], &buffer, sizeof(buffer));	// the process waits here
		printf("%d: received pong, which is %d\n", mypid, buffer);
		
		close(parent_fd[1]);
	} 
	else if (cpid == 0)	/* Child Process */
	{ 
		
		mypid = getpid();
		// [2] the child receives it
		read(parent_fd[0], &buffer, sizeof(buffer));	// the process waits here
		printf("%d: received ping, which is %d\n", mypid, buffer);
		
		// [3] the child responds with its own byte
		buffer = mypid;
		write(child_fd[1], &buffer, sizeof(buffer));
		
		close(child_fd[1]);	
	} 
	else 
	{
		printf("Fork failed\n");
	}
	
  
  exit(0);
  
  
}


