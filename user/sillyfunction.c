#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


int main(int argc, char *argv[]) 
{
	int cpid, mypid;
	int pid = getpid(); /* get current process PID */
	char temp = 10;	// used to verify the parent and child are executing with different memory
	printf("Parent pid is [%d]\n", pid);
	
	// Print the input arguments
	int i;
	printf("argc is [%d]\n", argc);
  for(i = 0; i < argc; i++)
	{	
		//char argv[] would mean a single string, char *argv[] means many strings.
		printf("argv[%d] is [%s]\n", i, argv[i]);
	}
  
  
  char* str1 = "I am AAA;";
  printf("str1 is [%s], strlen(str1) is [%d]\n", str1, strlen(str1));
  
  
  // test the function of file descriptor
  /*int fd;
  fd = open("README", O_RDONLY);
  printf("fd is [%d]\n", fd);
  close(fd);
	fd = open("README", O_RDONLY);
  printf("fd is [%d]\n", fd);
  fd = open("README", O_RDONLY);
  printf("fd is [%d]\n", fd);*/
  //fprintf(1, "read error\n");
  int fd, fd2;
  struct stat st, st2;
  fd = open("README", O_RDONLY);
  fstat(fd, &st);
  printf("dev:[%d], ino:[%d], type:[%d], nlink:[%d], size:[%d], fd:[%d]\n", st.dev, st.ino, st.type, st.nlink, st.size, fd);
  link("README", "b");
  fd2 = open("b", O_RDONLY);
  fstat(fd2, &st2);
  printf("dev:[%d], ino:[%d], type:[%d], nlink:[%d], size:[%d], fd:[%d]\n", st2.dev, st2.ino, st2.type, st2.nlink, st2.size, fd2);
  fd2 = open("sleep", O_RDONLY);
  fstat(fd2, &st2);
  printf("dev:[%d], ino:[%d], type:[%d], nlink:[%d], size:[%d], fd:[%d]\n", st2.dev, st2.ino, st2.type, st2.nlink, st2.size, fd2);
  //link("a", "b");
  
  
	cpid = fork();
	
	if (cpid > 0)	/* Parent Process */
	{ 
		// in case these processes are sliced by time
		pid = wait(0);	
		printf("Child process [%d] is done\n", pid);
		
		temp -=1;
		mypid = getpid();
		printf("Here is parent process [%d] from cpid [%d]\n", mypid, cpid);
		printf("Parent process [%d] change temp to [%d]\n", mypid, temp);
	} 
	else if (cpid == 0)	/* Child Process */
	{ 
		temp -=1;
		mypid = getpid();
		printf("Here is child process [%d] from cpid [%d]\n", mypid, cpid);
		printf("Child process [%d] change temp to [%d]\n", mypid, temp);
	} 
	else 
	{
		printf("Fork failed\n");
	}
	
	exit(0);
}




