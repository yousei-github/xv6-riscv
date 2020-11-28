#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BUFFER_HEAD_BYTES (4)
#define BUFFER_LENGTH (40)


typedef struct 
{
	int length;	// first 4 bytes are used for length
	unsigned char buffer[BUFFER_LENGTH];
} Buffer_Type;



int main(int argc, char *argv[])
{
  int cpid;
  int pipe_buffer[2];
	
	
	pipe(pipe_buffer);
  //printf("pipe_buffer[0] is %d, pipe_buffer[1] is %d.\n", pipe_buffer[0], pipe_buffer[1]);
	cpid = fork();
	
	if (cpid > 0)	/* Parent Process */
	{ 
		int i;
		Buffer_Type my_buffer;
		
		for (i = 2; i <= 35; i++)	// the first process feeds the numbers 2 through 35 into the pipeline
		{
			my_buffer.buffer[i-2] = i;
		}
		my_buffer.length = i - 2;
		
		//printf("length is %d and int is %d\n", my_buffer.length, sizeof(int));
		
		write(pipe_buffer[1], (char *)(&my_buffer), my_buffer.length + BUFFER_HEAD_BYTES);
		close(pipe_buffer[1]);	// it’s important for the parent to close the write end of the pipe
		wait(0);	// wait until child exits (must!!!)
		close(pipe_buffer[0]);
	} 
	else if (cpid == 0)
	while (cpid == 0)	/* Child Process */
	{ 
		int i;
		char *address;
		Buffer_Type my_buffer;
		char prime;
		int count;
		
		address = (char *)(&my_buffer);
		close(pipe_buffer[1]);	// it’s important for the child to close the write end of the pipe
		while(1)
		{
			i = read(pipe_buffer[0], address, sizeof(my_buffer));
			address += i;	// update offset
			// below check whether read() return 0, which means that all file descriptors referring to the write end have been closed
			//printf("i = %d\n", i);	
			if(i == 0)
				break;
		}
		close(pipe_buffer[0]);	// read end of pipe closes because it got all data
		
		/*
		for (i = 0; i < my_buffer.length; i++)	// use this to watch the received buffer
		{
			printf("my_buffer.buffer[%d] = %d\n", i, my_buffer.buffer[i]);
		}*/
		
		prime = my_buffer.buffer[0];	// get prime
		printf("prime %d\n", prime);
		count = 0;
		for (i = 1; i < my_buffer.length; i++)	// delete nonprimes
		{
			if (my_buffer.buffer[i] % prime != 0)
			{
				my_buffer.buffer[count] = my_buffer.buffer[i];
				count++;
			}
		}
		my_buffer.length = count;	// update length
		
		/*
		for (i = 0; i < my_buffer.length; i++)	// use this to watch the changed buffer
		{
			printf("my_buffer.buffer[%d] = %d\n", i, my_buffer.buffer[i]);
		}*/
		if (count != 0)	// create the next process
		{
			pipe(pipe_buffer);	// new pipe
			cpid = fork();
			if (cpid > 0)
			{
				// note here don't need to open pipe_buffer[1]
				write(pipe_buffer[1], (char *)(&my_buffer), my_buffer.length + BUFFER_HEAD_BYTES);
				close(pipe_buffer[1]);
				wait(0);	// parent must wait
				close(pipe_buffer[0]);
			}
			// child continues to execute
		}
		else
		{
			break;	// get all primes
		}
	} 
	else 
	{
		printf("Fork failed\n");
	}
	
  
  exit(0);
  
  
}



